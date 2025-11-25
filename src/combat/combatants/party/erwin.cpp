#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <random>
#include <cmath>
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
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
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
  else if (ai_goal == ErwinGoals::TARGETING){
    targetingBehavior();
  }
}

void Erwin::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  PartyMember::evaluateEvent(event);
  
  bool from_itself = event->sender == this;

  if (!from_itself && event->event_type == CombatantEVT::WARNING) {
    WarningCBT *warn_event = static_cast<WarningCBT*>(event.get());
    warningHandling(warn_event);
  }
}

void Erwin::warningHandling(WarningCBT *event) {
  assert(event->sender != this);
  if (ai_goal == ErwinGoals::DODGING) {
    return;
  }

  bool from_target = false;
  if (target != NULL) {
    float distance = distanceTo(target);
    bool contested = distance <= contest_distance;

    from_target = contested && target == event->assailant;
  }

  bool in_range = CheckCollisionRecs(hurtbox.rect, event->hitbox);

  if (!from_target && !in_range) {
    return;
  }
  
  PLOGI << "Acknowledging Warning sent by Entity [ID: " 
    << event->sender->entity_id << "]";

  float dodge_chance = chanceCalculation(event, from_target, in_range);
  PLOGI << "Chance to dodge attack: " << dodge_chance;

  setGoal(ErwinGoals::DODGING, dodge_chance);
  if (ai_goal != ErwinGoals::DODGING) {
    return;
  }

  PLOGI << "Decided to dodge the attack.";
  dodge_time = event->time_until * 0.90;
  dodge_clock = 0.0;

  if (target == NULL) {
    chooseTarget();
  }
}

float Erwin::chanceCalculation(WarningCBT *event, bool from_target,
                               bool in_range)
{
  float range = event->hitbox.width;
  float distance = distanceTo(event->sender);

  assert(range != 0);
  float range_bonus = std::sinf(distance / range) * 0.5;
  PLOGD << "Range Bonus: " << range_bonus;

  bool life_attack = event->action_type == ActionType::OFFENSE_HP;
  float time_bonus = event->time_until * life_attack;
  PLOGD << "Time Bonus: " << time_bonus;

  float multiplier = 1.0 - (0.5 * from_target);
  PLOGD << "Multiplier: " << multiplier;

  return (time_bonus + range_bonus) * multiplier;
}

void Erwin::rootBehavior() {
  bool enemies_present = Enemy::memberCount() != 0;
  if (enemies_present && player->target != NULL) {
    chooseTarget();
  }

  if (target != NULL) {
    PLOGI << "Now targeting: '" << target->name << "' [ID: " 
      << target->entity_id << "]";
    ai_goal = ErwinGoals::TARGETING;
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

void Erwin::targetingBehavior() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
    return;
  }

  if (waiting) {
    return;
  }

  tick_clock += Game::deltaTime();
  if (tick_clock < 1.0) {
    return;
  }

  tick_clock = 0.0;

  float distance = distanceTo(target);
  if (distance > contest_distance) {
    return;
  }

  setGoal(ErwinGoals::RETREATING, 0.40);
  if (ai_goal == ErwinGoals::RETREATING) {
    PLOGI << "Deciding to retreat from target.";
    uniform_real_distribution<float> range(0.10, 0.40);
    retreat_time = range(Game::RNG);
    return;
  }

  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);
  if (percentage <= 0.40) {
    wait(0.10, 0.25);
  }
}

void Erwin::chooseTarget() {
  int count = Enemy::memberCount();
  assert(count != 0);

  if (count == 1 && player->target != NULL) {
    target = player->target;
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

  if (!enemies.empty()) {
    auto closest = std::min_element(enemies.begin(), enemies.end(),
                                    Comparison::combatantDistance);
    target = closest->second;
  } 
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

void Erwin::ghoststep(int direction_x) {
  unique_ptr<CombatAction> action;
  action = make_unique<GhostStep>(this, atlas, direction_x, gs_set);
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
    case ErwinGoals::DODGING: {
      dodgingLogic();
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
  float difference = position.x - target->position.x;
  if (difference > 0) {
    direction = LEFT;
    moving_x = LEFT;
  }
  else {
    direction = RIGHT;
    moving_x = RIGHT; 
  }

  if (waiting) {
    waitTimer();
    return;
  }

  float distance = distanceTo(target);
  if (distance > attack_distance) {
    movement();
    return;
  }

  decideAttack();
  setGoal(ErwinGoals::RETREATING, 0.25);

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
  else {
    wait(0.10, 0.50);
  }

  retreat_clock = 0.0;
}

void Erwin::dodgingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    PLOGI << "Aborting dodging goal.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
    return;
  }

  dodge_clock += Game::deltaTime() / dodge_time;

  if (dodge_clock < 0.75) {
    return;
  }

  int x_direction;
  float difference = position.x - target->position.x;
  if (difference > 0) {
    direction = LEFT;
    x_direction = RIGHT;
  }
  else {
    direction = RIGHT;
    x_direction = LEFT;
  }

  if (dodge_clock < 1.0) {
    return;
  }

  ghoststep(x_direction);

  setGoal(ErwinGoals::TARGETING, 0.80);
  if (ai_goal != ErwinGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
  }
  else {
    wait(0.25, 0.50);
  }

  dodge_clock = 0.0;
}

void Erwin::wait(float time) {
  wait_time = time;
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Waiting for: " << wait_time << " seconds.";
}

void Erwin::wait(float min, float max) {
  uniform_real_distribution<float> range(min, max);

  wait_time = range(Game::RNG);
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Waiting for: " << wait_time << " seconds.";
}

void Erwin::waitTimer() {
  wait_clock += Game::deltaTime() / wait_time;

  if (wait_clock >= 1.0) {
    PLOGI << "Erwin is done waiting.";
    wait_clock = 0.0;
    waiting = false;
  }
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
