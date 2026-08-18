#include <cassert>
#include <cstddef>
#include <set>
#include <algorithm>
#include <utility>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/projectile.h"
#include "base/combat_action.h"
#include "data/combat_event.h"
#include "utils/comparisons.h"
#include "system/sprite_atlas.h"
#include "combat/system/evt_handler.h"
#include "combat/sub_weapons/bat.h"
#include "combat/actions/bat_swing.h"
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
}

void Baseball::update() {
  ownerCheck();

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

  bool using_action = owner != NULL && owner->state == ACTION;
  if (!using_action) {
    return;
  }

  assert(owner->action != nullptr);
  if (owner->action->id != ActionID::BAT_SWING) {
    return;
  }

  BatSwing *action = static_cast<BatSwing*>(owner->action.get());
  if (action->phase != ActionPhase::ACTIVE) {
    return;
  }

  Rectangle *swing_hitbox = &action->hitbox.rect;
  if (CheckCollisionRecs(*swing_hitbox, hitbox.rect)) {
    PLOGI << "Detected that BatSwing has hit the projectile.";
    swingSuccessful();
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

  trajectory.clear();
  launch(400, -90 + angle_offset);
  predictTrajectory(0.025);
  detectOncoming();

  sfx->play("bat_swing_hit");
  hit_by_swing = true;
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
}

void Baseball::onEndLife() {
  Projectile::onEndLife();
  sfx->play("baseball_grounded");
}

void Baseball::draw() {
  drawSprite(&atlas.sheet);
}
