#include <cassert>
#include <cstddef>
#include <set>
#include <algorithm>
#include <utility>
#include <raylib.h>
#include <raymath.h>
#include "combat/combatants/party/mary.h"
#include "enums.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/projectile.h"
#include "base/combat_action.h"
#include "data/combat_event.h"
#include "utils/comparisons.h"
#include "system/sprite_atlas.h"
#include "combat/system/evt_handler.h"
#include "combat/system/stage.h"
#include "combat/sub_weapons/bat.h"
#include "combat/actions/bat_swing.h"
#include "combat/actions/tail_whip.h"
#include "combat/combatants/party/xander.h"
#include "combat/projectiles/baseball.h"
#include <plog/Log.h>

using std::set, std::pair;
SpriteAtlas Baseball::atlas("projectiles", "baseball");


Baseball::Baseball(Combatant *owner, Vector2 position) : 
  Projectile("Baseball", owner, position)
{
  assert(atlas.users() > 0);
  id = ProjectileID::BASEBALL;
  sprite = &atlas.sprites[0];

  sfx = &Bat::sfx;
  assert(sfx->users() > 0);

  bounding_box.scale = {16, 16};
  bounding_box.offset = {-8, -8};

  hitbox.scale = {8, 8};
  hitbox.offset = {-4, -4};
  rectExCorrection(bounding_box, hitbox);

  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 0.10;
  data.stun_type = StunType::NORMAL;

  data.assailant = owner;
  data.hitbox = &hitbox.rect;
  data.negate_pushback = true;

  PLOGD << "Initial ATK: " << owner->dexterity;
  atk = owner->dexterity;
  data.a_atk = &atk;

  terminal_velocity = 200;
  gravity = 1.5;
  drag = 50;

  max_collisions = 1;

  int owner_direction = owner->direction;
  float angle_offset = 3 * owner_direction;
  launch(300, -90 + angle_offset);
  predictTrajectory(0.25);
  findXander();
}

void Baseball::findXander() {
  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant == owner) {
      continue;
    }

    if (combatant->state == DEAD) {
      continue;
    }

    if (combatant->team != CombatantTeam::PARTY) {
      continue;
    }

    PartyMember *member = static_cast<PartyMember*>(combatant);
    if (member->id == PartyMemberID::XANDER) {
      PLOGI << "Found Xander PartyMember";
      xander = static_cast<Xander*>(member);
    }
  }
}

void Baseball::update() {
  ownerCheck();

  if (end_crit_effect) {
    critEnd();
  }

  if (use_crit_effect) {
    critEffect();
    return;
  }

  if (dying) {
    deathTimer();
    return;
  }

  Vector2 old_position = position;
  runPhysics();
  lifeTimer();

  swingDetection();

  if (hit_by_swing || direction.y > 0) {
    distance_traveled += Vector2Distance(old_position, position);
    afterimages();
    hitRegistration(hits);
  }

  if (!hits.empty()) {
    inflictDamage(hits);
    onEndLife();
    death_time = 0.10;
  }

  warningProcess();
}

void Baseball::swingDetection() {
  if (hit_by_swing && direction.y < 0) {
    return;
  }

  bool mary_hit = checkMary();

  bool xander_hit = false;
  if (xander != NULL) {
    xander_hit = checkXander();
  }

  if (mary_hit || xander_hit) {
    trajectory.clear();
    predictTrajectory(0.025);
    detectOncoming();
    hit_by_swing = true;
  }

  if (mary_hit && xander_hit) {
    criticalHit();
  }
}

bool Baseball::checkMary() {
  bool using_action = owner != NULL && owner->state == ACTION;
  if (!using_action) {
    return false;
  }

  assert(owner->action != nullptr);
  if (owner->action->id != ActionID::BAT_SWING) {
    return false;
  }

  BatSwing *action = static_cast<BatSwing*>(owner->action.get());
  if (action->clashed || action->phase != ActionPhase::ACTIVE) {
    return false;
  }

  Rectangle *swing_hitbox = &action->hitbox.rect;
  if (CheckCollisionRecs(*swing_hitbox, hitbox.rect)) {
    PLOGI << "Detected that BatSwing has hit the projectile.";
    swingSuccessful();
    return true;
  }
  else {
    return false;
  }
}

bool Baseball::checkXander() {
  assert(xander != NULL);
  bool using_action = xander->state == ACTION;
  if (!using_action) {
    return false;
  }

  assert(xander->action != nullptr);
  if (xander->action->id != ActionID::XANDER_TAILWHIP) {
    return false;
  }

  TailWhip *action = static_cast<TailWhip*>(xander->action.get());
  switch (action->phase) {
    case ActionPhase::WIND_UP: {
      if (action->state_clock < 0.80) {
        return false;
      }
    }
    case ActionPhase::ACTIVE: {
      break;
    }
    case ActionPhase::END_LAG: {
      return false;
    }
  }

  Rectangle *whip_hitbox = &action->whip_hitbox.rect;
  if (CheckCollisionRecs(*whip_hitbox, hitbox.rect)) {
    PLOGI << "Detected that TailWhip has hit the projectile.";
    whipSuccessful();
    return true;
  }
  else {
    return false;
  }
}

void Baseball::swingSuccessful() {
  assert(!hit_by_swing);
  PLOGD << "Boosting the projectile's atk by: " << owner->offense;
  atk += owner->offense;
  data.stun_time = 0.35;

  int owner_direction = owner->direction;
  float angle_offset = 85 * owner_direction;

  terminal_velocity = 300;
  gravity = 0.5;
  drag = 25;

  launch(400, -90 + angle_offset);
  sfx->play("bat_swing_hit");
}

void Baseball::whipSuccessful() {
  assert(!hit_by_swing);
  PLOGD << "Boosting a projectile's atk by: " << xander->offense;
  atk += xander->offense;
  data.stun_time = 0.50;

  int owner_direction = owner->direction;
  float angle_offset = 85 * owner_direction;

  terminal_velocity = 450;
  gravity = 0.35;
  drag = 25;

  launch(550, -90 + angle_offset);
}

void Baseball::criticalHit() {
  assert(!use_crit_effect && !end_crit_effect);
  PLOGI << "Mary and Xander have landed a Critical Hit!";
  CombatStage::tintStage(Game::palette[48]);
  CombatHandler::raise<SetBarCB>(CombatEVT::BAR_SET, 0.0f, 24.0f);

  assert(owner != NULL && owner->action != nullptr);
  if (owner->action->id == ActionID::BAT_SWING) {
    BatSwing *action = static_cast<BatSwing*>(owner->action.get());
    action->data.hit_stop = 0;
  }

  assert(xander != NULL && xander->action != nullptr);
  if (xander->action->id == ActionID::XANDER_TAILWHIP) {
    TailWhip *action = static_cast<TailWhip*>(xander->action.get());
    action->data.hit_stop = 0;
  }

  owner->sprite = &Mary::atlas.sprites[44];
  owner->intangible = true;
  xander->intangible = true;

  data.stun_type = StunType::STAGGER;
  use_crit_effect = true;

  Combatant::sfx.play("evade_perfect");
  Game::sleep(0.375);
  Game::bgm->pause();
}

void Baseball::critEffect() {
  assert(xander != NULL);
  CombatStage::tintStage(Game::palette[48]);

  assert(xander->action != NULL);
  if (xander->action->phase == ActionPhase::ACTIVE) {
    CombatHandler::raise<SetBarCB>(CombatEVT::BAR_SET, 0.0f, 48.0f);
    end_crit_effect = true;

    sfx->play("bat_swing_hit", 1.20);
    Game::sleep(0.466);
  }
}

void Baseball::critEnd() {
  CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 1);
  Game::bgm->resume();

  sfx->play("bat_swing_clash");
  Combatant::sfx.play("technical");

  if (owner->action->id != ActionID::GHOST_STEP) {
    owner->intangible = false;
  }

  xander->intangible = false;

  use_crit_effect = false;
  end_crit_effect = false;
}

void Baseball::afterimages() {
  if (distance_traveled < 2) {
    return;
  }

  CombatHandler::raise<CreateAfterImgCB>(CombatEVT::CREATE_AFTERIMAGE,
                                         &atlas, sprite, 
                                         bounding_box.position, 
                                         RIGHT);
  distance_traveled = 0;
}

void Baseball::hitRegistration(set<pair<float, Combatant*>> &hits) {
  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->intangible) {
      continue;
    }

    if (combatant->team == alignment) {
      continue;
    }

    if (combatant->state == CombatantState::DEAD) {
      continue;
    }

    if (CheckCollisionRecs(hitbox.rect, combatant->hurtbox.rect)) {
      float distance = combatant->distanceTo(this);
      hits.emplace(std::make_pair(distance, combatant));
      PLOGD << "Projectile hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void Baseball::inflictDamage(set<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());

  Combatant *victim;
  if (hits.size() > 1) {
    auto closest = std::min_element(hits.begin(), hits.end(), 
                                    Comparison::combatantPriority);
    victim = closest->second;
  }
  else {
    victim = hits.begin()->second;
  }

  PLOGD << "Victim selected: '" << victim->name << "' [ID: " << 
    victim->entity_id << "]";
  victim->takeDamage(data);

  if (!data.intercepted && data.stun_type == StunType::STAGGER) {
    Combatant::sfx.play("damage_stagger");
  }
}

void Baseball::onEndLife() {
  Projectile::onEndLife();
  sfx->play("baseball_grounded");
}

void Baseball::draw() {
  drawSprite(&atlas.sheet);
}
