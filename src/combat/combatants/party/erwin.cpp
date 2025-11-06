#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <random>
#include <utility>
#include <set>
#include <raylib.h>
#include <raymath.h>
#include "base/combatant.h"
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/enemy.h"
#include "base/combat_action.h"
#include "data/session.h"
#include "data/animation.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/erwin.h"
#include <plog/Log.h>

using std::uniform_real_distribution, std::make_unique, std::unique_ptr;
SpriteAtlas Erwin::atlas("combatants", "erwin_combatant");


Erwin::Erwin(Companion *data, Mary *player): 
  PartyMember("Erwin", PartyMemberID::ERWIN, {-96, 152}, &atlas)
{
  this->player = player;

  life = data->life;
  max_life = data->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = data->init_morale;
  morale = init_morale;
  max_morale = data->max_morale;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;

  resilience = 0.60;

  afflictPersistent(data->status);

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {12, 56};
  hurtbox.offset = {-6, -58};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];
}

Erwin::~Erwin() {
  atlas.release();
}

void Erwin::setEnabled(bool value) {
  PartyMember::setEnabled(value);

  ai_goal = ErwinGoals::IDLE;
  tick_clock = 0;
}

void Erwin::behavior() {
  Combatant::behavior();

  if (!enabled) {
    return;
  }

  if (ai_goal == ErwinGoals::IDLE) {
    rootBehavior();
  }

  // Remove this later!
  if (state == NEUTRAL && IsKeyPressed(KEY_T)) {
    attackMP();
  }
  else if (state == NEUTRAL && IsKeyPressed(KEY_Y)) {
    attackHP();
  }
}

void Erwin::rootBehavior() {
  bool enemies_present = Enemy::memberCount() != 0;
  if (enemies_present && player->target != NULL) {
    chooseTarget();
    return;
  }

  float plr_distance = distanceTo(player);
  if (plr_distance > preferred_plr_distance) {
    ai_goal = ErwinGoals::FOLLOW_PLR;
    return;
  }

  tick_clock += Game::deltaTime();
  if (tick_clock >= 1.0) {
    setGoal(ErwinGoals::LOOK_AT_PLR, 0.25);
    tick_clock = 0.0;
  }
}

void Erwin::chooseTarget() {
  int count = Enemy::memberCount();
  assert(count != 0);
  assert(player->target != NULL);

  if (count == 1) {
    target = player->target;
    ai_goal = ErwinGoals::TARGETING;
    PLOGI << "Targeting the only remaining enemy: '" << target->name << 
      "' [ID: " << target->entity_id << "]"; 
    return;
  }

  std::set<std::pair<float, Combatant*>> enemies;

  for (Combatant *combatant : existing_combatants) {
    if (team == combatant->team) {
      continue;
    }

    if (combatant->state == CombatantState::DEAD) {
      continue;
    }

    if (player->target != combatant) {
      float distance = distanceTo(combatant);
      enemies.emplace(std::make_pair(distance, combatant));
    }
  }

  assert(!enemies.empty());
  auto closest = std::min_element(enemies.begin(), enemies.end(),
                                  Comparison::combatantDistance);
  target = closest->second;
  ai_goal = ErwinGoals::TARGETING;

  PLOGI << "Targeting the closest enemy that the player isn't "
    " targeting : '" << target->name << "' [ID: " << target->entity_id <<
  "]";
}

void Erwin::decideAttack() {
  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  float chance = 0.15;
  if (morale >= max_morale) {
    chance += 0.50;
  }

  if (morale >= 4 && percentage <= chance) {
    attackHP();
    morale -= 4;
  }
  else {
    attackMP();
  }
}

void Erwin::attackMP() {
  RectEx hitbox;
  hitbox.scale = {28, 8};
  hitbox.offset = {-14 + (14.0f * direction), -50};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, atlas, hitbox, atk_mp_set);
  performAction(action);
}

void Erwin::attackHP() {
  RectEx hitbox;
  hitbox.scale = {30, 8};
  hitbox.offset = {-15 + (16.0f * direction), -45};

  DamageData data;
  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;
  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;
  data.knockback = 45.0;
  data.hit_stop = 0.2;
  data.assailant = this;

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, ActionType::OFFENSE_HP, 0.20, 0.05,
                               0.25, hitbox, data, atlas, atk_hp_set);
  performAction(action);
}

void Erwin::setGoal(ErwinGoals goal, float chance) {
  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  if (percentage <= chance) {
    ai_goal = goal;
  }
}

void Erwin::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

      neutralLogic();
      break;
    }
    case CombatantState::ACTION: {
      action->logic();
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      sprite = getStunSprite();
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();

      SpriteAnimation::play(animation, &anim_dead, false);
      sprite = &atlas.sprites[*animation->current];
      break;
    }
  }

  statusLogic();
}

void Erwin::neutralLogic() {
  float old_x = position.x;

  switch (ai_goal) {
    case ErwinGoals::IDLE: {
      moving_x = 0;
      break;
    }
    case ErwinGoals::LOOK_AT_PLR: {
      lookAtPlayer();
      ai_goal = ErwinGoals::IDLE;
      break;
    }
    case ErwinGoals::FOLLOW_PLR: {
      followPlayer();
      break;
    }
    case ErwinGoals::TARGETING: {
      targetingLogic();
      break;
    }
    case ErwinGoals::RETREATING: {
      retreatingLogic();
      break;
    }
  }

  has_moved = old_x != position.x;
  animationLogic();
}

void Erwin::lookAtPlayer() {
  float difference = position.x - player->position.x;
  if (difference > 0) {
    direction = LEFT;
  }
  else {
    direction = RIGHT; 
  }
}

void Erwin::followPlayer() {
  float difference = position.x - player->position.x;
  if (difference > 0) {
    moving_x = LEFT;
  }
  else {
    moving_x = RIGHT;
  }

  movement();

  float distance = distanceTo(player);
  if (distance <= preferred_plr_distance / 2) {
    ai_goal = ErwinGoals::IDLE;
    moving_x = 0;
  }
}

void Erwin::targetingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
    return;
  }

  float difference = position.x - target->position.x;
  if (difference > 0) {
    moving_x = LEFT;
  }
  else {
    moving_x = RIGHT; 
  }

  float distance = distanceTo(target);
  if (distance > preferred_distance) {
    movement();
    return;
  }

  decideAttack();
  setGoal(ErwinGoals::RETREATING, 0.5);

  if (ai_goal == ErwinGoals::RETREATING) {
    PLOGI << "Retreating from target.";
    uniform_real_distribution<float> range(0.10, 0.75);
    retreat_time = range(Game::RNG);
  }
}

void Erwin::retreatingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
    return;
  }

  float difference = position.x - target->position.x;
  if (difference > 0) {
    moving_x = RIGHT;
  }
  else {
    moving_x = LEFT; 
  }

  movement();

  retreat_clock += Game::deltaTime() / retreat_time;
  if (retreat_clock < 1.0) {
    return;
  }

  setGoal(ErwinGoals::TARGETING, 0.50);
  if (ai_goal != ErwinGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
  }

  retreat_clock = 0.0;
}

void Erwin::movement() {
  if (moving_x == 0) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * speed_multiplier;
  float magnitude = speed * direction;

  position.x += magnitude * Game::deltaTime();
}

void Erwin::animationLogic() {
  Animation *next_anim;
  if (has_moved) {
    
    float difference = 1.0 - speed_multiplier;
    float percentage = 1.0 + difference;

    next_anim =  &anim_move;
    next_anim->frame_duration = anim_move_speed * percentage;
    rectExCorrection(bounding_box, hurtbox);
  }
  else {
    next_anim = getIdleAnim();
  }

  SpriteAnimation::play(animation, next_anim, true);
  sprite = &atlas.sprites[*animation->current];
}

Animation *Erwin::getIdleAnim() {
  if (!critical_life) {
    return &anim_idle;
  }
  else {
    return &anim_crit;
  }
}

Rectangle *Erwin::getStunSprite() {
  if (damage_type == DamageType::LIFE) {
    return &atlas.sprites[7];
  }
  else {
    return &atlas.sprites[8];
  }
}

void Erwin::draw() {
  assert(sprite != NULL);

  Rectangle final = *sprite;
  final.width = final.width * direction;

  applyStaggerEffect(final);
  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Erwin::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
