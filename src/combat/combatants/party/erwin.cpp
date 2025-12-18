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
#include "data/keybinds.h"
#include "data/animation.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "data/ai_behavior.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
#include "combat/actions/evade.h"
#include "combat/actions/provoke.h"
#include "combat/actions/3rd_party.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/erwin.h"
#include <plog/Log.h>

using std::uniform_real_distribution, std::make_unique, std::unique_ptr;
SpriteAtlas Erwin::atlas("combatants", "erwin_combatant");
SoundAtlas Erwin::psfx("erwin");


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
  ai_behavior = make_unique<ErwinAI>();

  tech1 = {"Provoke", TechCostType::MORALE, 8};
  tech1.cooldown = 5.0;

  tech2 = {"3rd Party", TechCostType::MORALE, 10};
  tech2.cooldown = 8.0;

  afflictPersistent(data->status);

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {12, 56};
  hurtbox.offset = {-6, -58};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  psfx.use();

  sprite = &atlas.sprites[0];
  keybinds = &Game::settings.combat_keybinds;
}

Erwin::~Erwin() {
  ai_behavior.reset();
  atlas.release();
  psfx.release();
}

void Erwin::setEnabled(bool value) {
  PartyMember::setEnabled(value);

  ai_goal = ErwinGoals::IDLE;
  target = NULL;
  tick_clock = 0;
}

void Erwin::behavior() {
  Combatant::behavior();

  if (!enabled) {
    return;
  }

  assistInput();

  if (ai_goal == ErwinGoals::IDLE) {
    rootBehavior();
  }
  else if (ai_goal == ErwinGoals::TARGETING){
    targetingBehavior();
  }
}

void Erwin::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  PartyMember::evaluateEvent(event);
  
  if (!enabled) {
    return;
  }

  bool from_itself = event->sender == this;

  if (!from_itself && event->event_type == CombatantEVT::WARNING) {
    WarningCBT *warn_event = static_cast<WarningCBT*>(event.get());
    warningHandling(warn_event);
    return;
  }

  if (from_itself && event->event_type == CombatantEVT::TOOK_DAMAGE) {
    TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
    damageHandling(dmg_event);
    return;
  }
}

void Erwin::warningHandling(WarningCBT *event) {
  assert(event->sender != this);
  if (ai_goal == ErwinGoals::DODGING) {
    return;
  }

  if (team == event->assailant->team) {
    return;
  }

  if (life <= 1) {
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

  float retaliation_chance = ai_behavior->dodging.retaliation_chance;
  retaliation(event->assailant, retaliation_chance);

  if (target == NULL) {
    chooseTarget();
  }
}

void Erwin::damageHandling(TookDamageCBT *event) {
  if (event->resulting_state != HIT_STUN) {
    return;
  }

  if (event->assailant == target) {
    return;
  }

  float retaliation_chance = ai_behavior->retaliation_chance;
  retaliation(event->assailant, retaliation_chance);
}

void Erwin::retaliation(Combatant *assailant, float chance) {
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

float Erwin::chanceCalculation(WarningCBT *event, bool from_target,
                               bool in_range)
{
  float range = event->hitbox.width;
  float distance = distanceTo(event->sender);

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

void Erwin::assistInput() {
  bool gamepad = IsGamepadAvailable(0);
  bool light_input = Input::pressed(keybinds->light_assist, gamepad);
  if (light_input && lightAssistCondition()) {
    ai_goal = ErwinGoals::PROVOKE;
    tech1.clock = 0.0;
    sfx.play("assist_call");
  }

  bool heavy_input = Input::pressed(keybinds->heavy_assist, gamepad);
  if (heavy_input && heavyAssistCondition()) {
    ai_goal = ErwinGoals::THIRD_PARTY;

    target = player->target;
    assert(target != NULL);
    PLOGI << "Now targeting: '" << target->name << "' [ID: " 
      << target->entity_id << "]";

    tech2.clock = 0.0;
    sfx.play("assist_call");
  }
}

bool Erwin::lightAssistCondition() {
  bool off_cooldown = tech1.clock >= 1.0;
  if (off_cooldown && !demoralized && morale >= tech1.cost) {
    return true;
  }
  else {
    sfx.play("action_denied");
    return false;
  }
}

bool Erwin::heavyAssistCondition() {
  bool off_cooldown = tech2.clock >= 1.0;
  bool sufficent_morale = !demoralized && morale >= tech2.cost;
  bool valid = player->target != NULL && player->target->targetable;

  if (off_cooldown && sufficent_morale && valid) {
    return true;
  }
  else {
    sfx.play("action_denied");
    return false;
  }
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
  if (target->state == DEAD || !target->targetable) {
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

  float retreat_chance = ai_behavior->contesting.retreat_chance;
  setGoal(ErwinGoals::RETREATING, retreat_chance);
  if (ai_goal == ErwinGoals::RETREATING) {
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

void Erwin::chooseTarget() {
  int count = Enemy::memberCount();
  assert(count != 0);

  if (count == 1 && player->target != NULL) {
    target = player->target;
    return;
  }

  std::set<std::pair<float, Combatant*>> enemies;

  for (Combatant *combatant : existing_combatants) {
    if (combatant->state == CombatantState::DEAD) {
      continue;
    }

    if (!combatant->targetable || team == combatant->team) {
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
  if (morale >= max_morale * 0.75) {
    chance += 0.45;
  }

  bool sufficent = !demoralized && morale >= 4;

  if (sufficent && percentage <= chance) {
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
  increaseExhaustion(5.5);

  unique_ptr<CombatAction> action;
  action = make_unique<GhostStep>(this, atlas, direction_x, gs_set);
  performAction(action);
}

void Erwin::evade() {
  RectEx hitbox;
  hitbox.scale = {8, 46};
  hitbox.offset = {-4 + (8.0f * direction), -48};

  unique_ptr<CombatAction> action;
  action = make_unique<Evade>(this, atlas, hitbox, ev_set);
  performAction(action);
}

void Erwin::provoke() {
  if (morale < tech1.cost) {
    return;
  }

  morale -= tech1.cost;

  unique_ptr<CombatAction> action;
  action = make_unique<Provoke>(this);
  performAction(action);
}

void Erwin::thirdparty() {
  if (morale < tech2.cost) {
    return;
  }

  morale -= tech2.cost;

  unique_ptr<CombatAction> action;
  action = make_unique<ThirdParty>(this);
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
  techniqueCooldown();
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
    case ErwinGoals::PROVOKE: {
      provoke();
      ai_goal = ErwinGoals::IDLE;
      break;
    }
    case ErwinGoals::THIRD_PARTY: {
      thirdPartyLogic();
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

  movement(speed_multiplier);

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
    movement(speed_multiplier);
    return;
  }

  decideAttack();

  float retreat_chance = ai_behavior->targeting.retreat_chance;
  setGoal(ErwinGoals::RETREATING, retreat_chance);

  if (ai_goal == ErwinGoals::RETREATING) {
    PLOGI << "Retreating from target.";
    float min_retreat = ai_behavior->targeting.min_retreat;
    float max_retreat = ai_behavior->targeting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
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

  movement(speed_multiplier);

  retreat_clock += Game::deltaTime() / retreat_time;
  if (retreat_clock < 1.0) {
    return;
  }

  float target_chance = ai_behavior->retreating.target_chance;
  setGoal(ErwinGoals::TARGETING, target_chance);

  if (ai_goal != ErwinGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai_behavior->retreating.min_wait;
    float max_wait = ai_behavior->retreating.max_wait;
    wait(min_wait, max_wait);
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

  if (dodge_clock < 0.50) {
    targetingLogic();
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

  float target_chance = ai_behavior->dodging.target_chance;
  setGoal(ErwinGoals::TARGETING, target_chance);

  if (ai_goal != ErwinGoals::TARGETING) {
    PLOGI << "Returning to idle.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
  }
  else {
    float min_wait = ai_behavior->dodging.min_wait;
    float max_wait = ai_behavior->dodging.max_wait;
    wait(min_wait, max_wait);
  }

  dodge_clock = 0.0;
}

void Erwin::thirdPartyLogic() {
  assert(target != NULL);
  if (target->state == DEAD || !target->targetable) {
    PLOGI << "Aborting Third Party goal.";
    ai_goal = ErwinGoals::IDLE;
    target = NULL;
    return;
  }

  float difference = position.x - target->position.x;
  if (difference > 0) {
    direction = LEFT;
    moving_x = LEFT;
  }
  else {
    direction = RIGHT;
    moving_x = RIGHT; 
  }

  float distance = distanceTo(target);
  if (distance > 96) {
    movement(speed_multiplier * 3);
    return;
  }

  thirdparty();
  ai_goal = ErwinGoals::IDLE;
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

void Erwin::movement(float multiplier) {
  if (moving_x == 0) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * multiplier;
  float magnitude = speed * direction;

  position.x += magnitude * Game::deltaTime();
}

void Erwin::animationLogic() {
  Animation *next_anim;
  if (has_moved) {  
    bool using_tech2 = ai_goal == ErwinGoals::THIRD_PARTY;
    float multiplier = 1.0 + (2 * using_tech2);

    float difference = 1.0 - (speed_multiplier * multiplier);
    float percentage = Clamp(1.0 + difference, 0.50, 10.0);

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
