#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <set>
#include <utility>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/ai_behavior.h"
#include "data/combatant_event.h"
#include "data/damage.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
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

  offense = 8;
  defense = 6;
  intimid = 6;
  persist = 6;
  dexterity = 4;
  discipline = 1;

  resilience = 0.8;
  ai_behavior = make_unique<ServantAI>();

  bounding_box.scale = {80, 80};
  bounding_box.offset = {-40, -80};
  hurtbox.scale = {20, 64};
  hurtbox.offset = {-10,-66};

  rectExCorrection(bounding_box, hurtbox);
  atlas.use();

  sprite = &atlas.sprites[0];
}

Servant::~Servant() {
  ai_behavior.reset();
  atlas.release();
}

void Servant::behavior() {
  if (ai_goal == ServantGoals::IDLE) {
    rootBehavior();
  }
  else if (ai_goal == ServantGoals::TARGETING) {
    targetingBehavior();
  }
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
  if (ai_goal == ServantGoals::DODGING) {
    return;
  }

  if (team == event->assailant->team) {
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

  PLOGI << "Servant [ID: " << entity_id << "] Acknowledging Warning sent"
    << " by Entity [ID: " << event->sender->entity_id << "]";


  float dodge_chance = chanceCalculation(event, from_target, in_range);
  PLOGI << "Chance to dodge attack: " << dodge_chance;

  setGoal(ServantGoals::DODGING, dodge_chance);
  if (ai_goal != ServantGoals::DODGING) {
    return;
  }

  PLOGI << "Servant [ID: " << entity_id << "] has decided to dodge the" 
    << " attack";
  dodge_time = event->time_until * 0.90;
  dodge_clock = 0.0;

  float retaliation_chance = ai_behavior->dodging.retaliation_chance;
  retaliation(event->assailant, retaliation_chance);

  if (target == NULL) {
    chooseTarget();
  }
}

void Servant::damageHandling(TookDamageCBT *event) {
  if (event->resulting_state != HIT_STUN) {
    return;
  }

  if (event->assailant != target) {
    float retaliation_chance = ai_behavior->damaged.retaliation_chance;
    retaliation(event->assailant, retaliation_chance);
  }

  if (event->damage_type != DamageType::LIFE) {
    return;
  }

  float retreat_chance = ai_behavior->damaged.retreat_chance;
  setGoal(ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "'" << name << "' [ID: " << entity_id << "] has decided to" 
    << "retreat after taking life damage.";
    float min_retreat = ai_behavior->damaged.min_retreat;
    float max_retreat = ai_behavior->damaged.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    retreat_time = range(Game::RNG);
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
  if (distance > contest_distance) {
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

float Servant::chanceCalculation(WarningCBT *event, bool from_target,
                                 bool in_range)
{
  float range = event->hitbox.width;
  PLOGD << "Range: " << range; 

  float distance = distanceTo(event->sender);
  PLOGD << "Distance from sender: " << distance;

  assert(range != 0);
  float range_multiplier = ai_behavior->dodging.range_multiplier;
  float range_bonus = std::sinf(distance / range) * range_multiplier;
  PLOGD << "Range Bonus: " << range_bonus;

  bool life_attack = event->action_type == ActionType::OFFENSE_HP;
  float time_multiplier = ai_behavior->dodging.time_multiplier;
  float time_bonus = (event->time_until * time_multiplier) * life_attack;
  PLOGD << "Time Bonus: " << time_bonus;

  float penalty = ai_behavior->dodging.penalty;
  float multiplier = 1.0 - (penalty * from_target);
  PLOGD << "Multiplier: " << multiplier;

  return (time_bonus + range_bonus) * multiplier;
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

  float retreat_chance = ai_behavior->contesting.retreat_chance;
  setGoal(ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "Deciding to retreat from target.";
    float min_retreat = ai_behavior->contesting.min_retreat;
    float max_retreat = ai_behavior->contesting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    retreat_time = range(Game::RNG);
    return;
  }

  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);
  float wait_chance = ai_behavior->contesting.wait_chance;

  if (percentage <= wait_chance) {
    float min_wait = ai_behavior->contesting.min_wait;
    float max_wait = ai_behavior->contesting.max_wait;
    wait(min_wait, max_wait);
  }
}

void Servant::chooseTarget() {
  int count = PartyMember::memberCount();
  assert(count != 0);

  std::set<std::pair<float, Combatant*>> party_members;

  for (Combatant *combatant : existing_combatants) {
    if (combatant->team != CombatantTeam::PARTY) {
      continue;
    }

    if (combatant->targetable && combatant->state != DEAD) {
      float distance = distanceTo(combatant);
      party_members.emplace(std::make_pair(distance, combatant));
    }
  }

  if (party_members.empty()) {
    return;
  }

  if (party_members.size() == 1) {
    target = party_members.begin()->second;
  }
  else {
    auto closest = std::min_element(party_members.begin(), 
                                    party_members.end(),
                                    Comparison::combatantDistance);

    target = closest->second;
  }
}

void Servant::setGoal(ServantGoals goal, float chance) {
  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  if (percentage <= chance) {
    ai_goal = goal;
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

  if (party_member->demoralized) {
    chance += 0.40;
  }

  if (percentage <= chance) {
    attackHP();
  }
  else {
    attackMP();
  }
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

  int direction_x;

  float difference = position.x - target->position.x;
  if (difference < 0) {
    direction_x = LEFT;
  }
  else {
    direction_x = RIGHT;
  }

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

void Servant::neutralLogic() {
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
    decelerate();
    waitTimer();
    return;
  }

  float distance = distanceTo(target);
  if (distance > attack_distance) {
    movement();
    return;
  }

  decideAttack();

  float retreat_chance = ai_behavior->targeting.retreat_chance;
  setGoal(ServantGoals::RETREATING, retreat_chance);

  if (ai_goal == ServantGoals::RETREATING) {
    PLOGI << "Retreating from target.";
    float min_retreat = ai_behavior->targeting.min_retreat;
    float max_retreat = ai_behavior->targeting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    retreat_time = range(Game::RNG);
  }
}

void Servant::retreatingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = ServantGoals::IDLE;
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

  float target_chance = ai_behavior->retreating.target_chance;
  setGoal(ServantGoals::TARGETING, target_chance);

  if (ai_goal != ServantGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai_behavior->retreating.min_wait;
    float max_wait = ai_behavior->retreating.max_wait;
    wait(min_wait, max_wait);  
  }

  retreat_clock = 0.0;
}

void Servant::dodgingLogic() {
  if (target == NULL || target->state == DEAD) {
    PLOGI << "Aborting dodging goal.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
    return;
  }

  if (dodge_clock < 0.50) {
    targetingLogic();
  }

  dodge_clock += Game::deltaTime() / dodge_time;

  if (dodge_clock < 0.75) {
    return;
  }

  if (acceleration != 0.0) {
    decelerate();
  }

  float difference = position.x - target->position.x;
  if (difference > 0) {
    direction = LEFT;
  }
  else {
    direction = RIGHT;
  }

  if (dodge_clock < 1.0) {
    return;
  }

  ghoststep();

  float target_chance = ai_behavior->dodging.target_chance;
  setGoal(ServantGoals::TARGETING, target_chance);

  if (ai_goal != ServantGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ServantGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai_behavior->dodging.min_wait;
    float max_wait = ai_behavior->dodging.max_wait;
    wait(min_wait, max_wait);
  }

  dodge_clock = 0.0;
}

void Servant::wait(float time) {
  wait_time = time;
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Servant [ID: " << entity_id << "] Waiting for: " << wait_time 
    << " seconds.";
}

void Servant::wait(float min, float max) {
  uniform_real_distribution<float> range(min, max);

  wait_time = range(Game::RNG);
  wait_clock = 0.0;
  waiting = true;
  PLOGI << "Servant [ID: " << entity_id << "] Waiting for: " << wait_time 
    << " seconds.";
}

void Servant::waitTimer() {
  wait_clock += Game::deltaTime() / wait_time;

  if (wait_clock >= 1.0) {
    PLOGI << "Servant [ID: " << entity_id << "] is done waiting.";
    wait_clock = 0.0;
    waiting = false;
  }
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
  float magnitude = speed * direction;

  position.x += magnitude * Game::deltaTime();
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
  assert(sprite != NULL);
  Rectangle final = *sprite;
  final.width = final.width * direction;

  applyStaggerEffect(final);
  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Servant::drawDebug() {
  Combatant::drawDebug();

  Font *font = &Game::sm_font;
  int size = font->baseSize;
  const char *txt_goal = TextFormat("%i", static_cast<int>(ai_goal));

  Vector2 txt_pos = Vector2Add(position, {0, 8});
  DrawTextEx(*font, txt_goal, txt_pos, size, -3, RED);

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
