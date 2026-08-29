#include <random>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "base/ai_parameters.h"
#include "data/combatant_event.h"
#include "data/damage.h"
#include "utils/animation.h"
#include "utils/collision.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
#include "combat/combatants/enemy/servant.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::uniform_real_distribution;
SpriteAtlas Servant::atlas("combatants", "servant_combatant");


Servant::Servant(Vector2 position, Direction direction) : 
  Enemy("Servant", EnemyID::SERVANT, position)
{
  max_life = 10;
  life = max_life;

  offense = 10;
  defense = 6;
  intimid = 8;
  persist = 2;
  dexterity = 4;
  discipline = 1;

  resilience = 0.8;
  z_order = 5;

  ai = make_unique<ServantAI>();

  bounding_box.scale = {80, 80};
  bounding_box.offset = {-40, -80};
  hurtbox.scale = {20, 64};
  hurtbox.offset = {-10,-66};

  rectExCorrection(bounding_box, hurtbox);
  atlas.use();

  sprite = &atlas.sprites[0];
}

Servant::~Servant() {
  ai.reset();
  atlas.release();
}

void Servant::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  Enemy::evaluateEvent(event);

  bool from_itself = event->sender == this;

  if (!from_itself && event->event_type == CombatantEVT::WARNING) {
    WarningCBT *warn_event = static_cast<WarningCBT*>(event.get());
    warningHandling(warn_event);
  }

  if (from_itself && event->event_type == CombatantEVT::TOOK_DAMAGE) {
    TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
    damageHandling(dmg_event);
    return;
  }
}

void Servant::warningHandling(WarningCBT *event) {
  assert(event->sender != this);

  bool from_target;
  bool in_range;
  if (!shouldAcknowledge(event, from_target, in_range)) {
    return;
  }

  PLOGI << "Servant [ID: " << entity_id << "] Acknowledging Warning sent"
    << " by Entity [ID: " << event->sender->entity_id << "]";

  float dodge_chance = chanceCalculation(event, from_target, in_range);
  PLOGI << "Chance to dodge attack: " << dodge_chance;

  ai->setGoal(ai_goal, ServantGoals::DODGING, dodge_chance);
  if (ai_goal == ServantGoals::DODGING) {
    PLOGI << "Servant [ID: " << entity_id << "] has decided to dodge the" 
      << " attack";
    warningReaction(event);
  }

  if (target == NULL) {
    chooseTarget();
  }
}

bool Servant::shouldAcknowledge(WarningCBT *event, bool &from_target,
                                bool &in_range)
{
  if (event->assailant == NULL || team == event->assailant->team) {
    return false;
  }

  if (ai_goal == ServantGoals::DODGING) {
    return false;
  }
  
  if (target != NULL) {
    float distance = distanceTo(target);
    bool contested = distance <= ai->contest_distance;

    from_target = contested && target == event->assailant;
  }

  in_range = CheckCollisionRecs(hurtbox.rect, event->hitbox);

  if (from_target || in_range) {
    return true;
  }
  else {
    return false;
  }
}

float Servant::chanceCalculation(WarningCBT *event, bool from_target,
                                 bool in_range)
{
  float range = event->hitbox.width;
  PLOGD << "Range: " << range; 

  float distance = distanceTo(event->sender);
  PLOGD << "Distance from sender: " << distance;

  assert(range != 0);
  float range_multiplier = ai->dodging.range_multiplier;
  float range_bonus = std::sinf(distance / range) * range_multiplier;
  PLOGD << "Range Bonus: " << range_bonus;

  bool life_attack = event->action_type == ActionType::OFFENSE_HP;
  float time_multiplier = ai->dodging.time_multiplier;
  float time_bonus = (event->time_until * time_multiplier) * life_attack;
  PLOGD << "Time Bonus: " << time_bonus;

  float penalty = ai->dodging.penalty;
  float multiplier = 1.0 - (penalty * from_target);
  PLOGD << "Multiplier: " << multiplier;

  return (time_bonus + range_bonus) * multiplier;
}

void Servant::warningReaction(WarningCBT *event) {
  ai->dodge_time = event->time_until * 0.90;
  ai->dodge_clock = 0.0;

  float retaliation_chance = ai->dodging.retaliation_chance;
  retaliation(event->assailant, retaliation_chance);
}

void Servant::damageHandling(TookDamageCBT *event) {
  if (event->resulting_state != HIT_STUN) {
    return;
  }

  if (event->assailant != target) {
    float retaliation_chance = ai->damaged.retaliation_chance;
    retaliation(event->assailant, retaliation_chance);
  }

  if (event->damage_type != DamageType::LIFE) {
    return;
  }

  float retreat_chance = ai->damaged.retreat_chance;
  ai->setGoal(ai_goal, ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "'" << name << "' [ID: " << entity_id << "] has decided to" 
    << "retreat after taking life damage.";
    float min_retreat = ai->damaged.min_retreat;
    float max_retreat = ai->damaged.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    ai->retreat_time = range(Game::RNG);
  }
}

void Servant::retaliation(Combatant *assailant, float chance) {
  if (assailant == NULL || assailant == target) {
    return;
  }

  if (!assailant->targetable || team == assailant->team) {
    return;
  }

  float distance = distanceTo(assailant);
  if (distance > ai->contest_distance) {
    return;
  }

  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  if (percentage > chance) {
    return;
  }

  target = assailant;

  PLOGI << "'" << name << "' [ID: " << entity_id << "] has decided to" 
  << "retaliate against: '" << target->name << "' [ID: " << 
    target->entity_id << "]";
}

void Servant::behavior() {
  if (ai_goal == ServantGoals::IDLE) {
    rootBehavior();
  }
  else if (ai_goal == ServantGoals::TARGETING) {
    targetingBehavior();
  }
}

void Servant::rootBehavior() {
  bool party_alive = PartyMember::memberCount() != 0;
  if (party_alive) {
    chooseTarget();
  }

  if (target != NULL) {
    PLOGI << "Servant [ID: " << entity_id << "] is now targeting: '" <<
    target->name << "' [ID: " << target->entity_id << "]";
    ai_goal = ServantGoals::TARGETING;
  }
}

void Servant::targetingBehavior() {
  assert(target != NULL);
  if (target->state == DEAD || !target->targetable) {
    ai_goal = ServantGoals::IDLE;
    target = NULL;
    return;
  }

  if (ai->waiting) {
    return;
  }

  tick_clock += Game::deltaTime();
  if (tick_clock < 1.0) {
    return;
  }

  tick_clock = 0.0;

  float distance = distanceTo(target);
  if (distance > ai->contest_distance) {
    return;
  }

  float retreat_chance = ai->contesting.retreat_chance;
  ai->setGoal(ai_goal, ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "Deciding to retreat from target.";
    float min_retreat = ai->contesting.min_retreat;
    float max_retreat = ai->contesting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    ai->retreat_time = range(Game::RNG);
    return;
  }

  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);
  float wait_chance = ai->contesting.wait_chance;

  if (percentage <= wait_chance) {
    float min_wait = ai->contesting.min_wait;
    float max_wait = ai->contesting.max_wait;
    ai->wait(min_wait, max_wait);
  }
}

void Servant::decideAttack() {
  assert(target->team == CombatantTeam::PARTY);
  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  PartyMember *party_member = static_cast<PartyMember*>(target);
  float chance = 0.20;

  if (party_member->critical_life) {
    chance += 0.20;
  }

  if (party_member->demoralized || party_member->max_morale == 0) {
    chance += 0.40;
  }

  if (percentage <= chance) {
    attackHP();
  }
  else {
    attackMP();
  }

  ai->attack_cooldown = 1.0;
  ai->cooldown_clock = 0.0;
}

void Servant::attackMP() {
  RectEx hitbox;
  hitbox.scale = {32, 12};
  hitbox.offset = {-16 + (16.0f * direction), -52};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, atlas, hitbox, atk_mp_set);
  performAction(action);
}

void Servant::attackHP() {
  RectEx hitbox;
  hitbox.scale = {38, 24};
  hitbox.offset = {-19 + (19.0f * direction), -52};

  DamageData data;
  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;
  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;
  data.knockback = 20.0;
  data.hit_stop = 0.2;
  data.assailant = this;

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, ActionType::OFFENSE_HP, 0.35, 0.05,
                               0.25, hitbox, data, atlas, atk_hp_set);

  performAction(action);
  sfx.play("enemy_warning1");
}

void Servant::ghoststep() {
  assert(target != NULL);

  int direction_x = directionTo(target) * -1;

  unique_ptr<CombatAction> action;
  action = make_unique<GhostStep>(this, atlas, direction_x, gs_set);
  performAction(action);
}

void Servant::update() {
  switch (state) {
    case CombatantState::NEUTRAL: {
      neutralLogic();
      break;
    }
    case CombatantState::ACTION: {
      action->logic();

      if (kb_push_back) {
        knockbackLogic();
      }
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      knockbackLogic();
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

  endLogic();
}

void Servant::neutralLogic() {
  if (ai->cooldown_clock < 1.0) {
    ai->cooldown_clock += Game::deltaTime() / ai->attack_cooldown;
  }

  float old_x = position.x;

  switch (ai_goal) {
    case ServantGoals::IDLE: {
      moving_x = 0;
      movement();
      break;
    }
    case ServantGoals::TARGETING: {
      targetingLogic();
      break;
    }
    case ServantGoals::RETREATING: {
      retreatingLogic();
      break;
    }
    case ServantGoals::DODGING: {
      dodgingLogic();
      break;
    }
  }

  has_moved = old_x != position.x;
  if (has_moved) {
    useMovingAnimation();
    rectExCorrection(bounding_box, hurtbox);
  }
  else {
    sprite = &atlas.sprites[0]; 
  }
}

void Servant::targetingLogic() {
  assert(target != NULL);

  direction = directionTo(target);
  moving_x = direction;

  if (ai->waiting) {
    decelerate();
    ai->waitTimer();
    return;
  }

  if (!ai->inAttackRange(this, target)) {
    movement();
    return;
  }

  if (ai->cooldown_clock < 1.0) {
    return;
  }

  decideAttack();

  float retreat_chance = ai->targeting.retreat_chance;
  ai->setGoal(ai_goal, ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "Retreating from target.";
    float min_retreat = ai->targeting.min_retreat;
    float max_retreat = ai->targeting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    ai->retreat_time = range(Game::RNG);
  }
}

void Servant::retreatingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = ServantGoals::IDLE;
    target = NULL;
    return;
  }

  moving_x = directionTo(target) * -1;
  movement();

  ai->retreat_clock += Game::deltaTime() / ai->retreat_time;
  if (ai->retreat_clock < 1.0) {
    return;
  }

  float target_chance = ai->retreating.target_chance;
  ai->setGoal(ai_goal, ServantGoals::TARGETING, target_chance);

  if (ai_goal != ServantGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai->retreating.min_wait;
    float max_wait = ai->retreating.max_wait;
    ai->wait(min_wait, max_wait);  
  }

  ai->retreat_clock = 0.0;
}

void Servant::dodgingLogic() {
  if (target == NULL || target->state == DEAD) {
    PLOGI << "Aborting dodging goal.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
    return;
  }

  if (ai->dodge_clock < 0.50) {
    targetingLogic();
  }

  ai->dodge_clock += Game::deltaTime() / ai->dodge_time;

  if (ai->dodge_clock < 0.75) {
    return;
  }

  if (acceleration != 0.0) {
    decelerate();
  }

  direction = directionTo(target);

  if (ai->dodge_clock < 1.0) {
    return;
  }

  ghoststep();

  float target_chance = ai->dodging.target_chance;
  ai->setGoal(ai_goal, ServantGoals::TARGETING, target_chance);

  if (ai_goal != ServantGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai->dodging.min_wait;
    float max_wait = ai->dodging.max_wait;
    ai->wait(min_wait, max_wait);
  }

  ai->dodge_clock = 0.0;
}

void Servant::movement() {
  if (moving_x == 0 && acceleration == 0) {
    return;
  }

  if (moving_x != 0) {
    direction = static_cast<Direction>(moving_x);
    accelerate();
  }
  else {
    decelerate();
  }

  float speed = (default_speed * speed_multiplier) * acceleration;
  float magnitude = speed * Game::deltaTime();

  if (Collision::checkX(this, magnitude, moving_x)) {
    Collision::snapX(this, moving_x);
  }
  else {
    position.x += magnitude * direction; 
  }
}

void Servant::useMovingAnimation() {
  float difference = 1.0 - (speed_multiplier * acceleration);
  float percentage = 1.0 + difference;

  anim_move.frame_duration = anim_move_speed * percentage;
  SpriteAnimation::play(animation, &anim_move, true);
  sprite = &atlas.sprites[*animation->current];
}

Rectangle *Servant::getStunSprite() {
  if (damage_type == DamageType::LIFE) {
    return &atlas.sprites[5];
  }
  else {
    return &atlas.sprites[4];
  }
}

void Servant::draw() {
  drawSprite(&atlas.sheet);
}

void Servant::drawDebug() {
  Combatant::drawDebug();
  ai->drawDebug(static_cast<int>(ai_goal), position, bounding_box);

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
