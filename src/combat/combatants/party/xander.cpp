#include <cassert>
#include <cstddef>
#include <memory>
#include <random>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/enemy.h"
#include "base/combat_action.h"
#include "data/session.h"
#include "data/animation.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/collision.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/actions/hand_blade.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/xander.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::uniform_real_distribution;
SpriteAtlas Xander::atlas("combatants", "xander_combatant");
SoundAtlas Xander::psfx("xander");


Xander::Xander(Companion *data, Mary *player) : 
  PartyMember("Xander", PartyMemberID::XANDER, {-96, 152}, &atlas)
{
  assert(id == data->member_id);
  this->player = player;

  life = data->life;
  max_life = data->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = data->init_morale;
  morale = init_morale;
  max_morale = data->max_morale;

  tenacity = 3.0;
  tp_natural = 0.1;
  tp_threshold = tp_natural;
  tp_regen_delay = 15;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;
  dexterity = data->dexterity;
  discipline = data->discipline;

  recovery = data->recovery;
  resilience = data->resilience;

  accel_rate = 0.125;
  decel_rate = 0.5;
  z_order = 2;

  ai = make_unique<XanderAI>();

  tech1 = {"Tail Whip", TechCostType::LIFE, 2.0};
  tech1.cooldown = 10.0;

  tech2 = {"Meteor", TechCostType::LIFE, 2.0};
  tech2.cooldown = 15.0;

  afflictPersistent(data->status);

  bounding_box.scale = {144, 128};
  bounding_box.offset = {-72, -128};
  hurtbox.scale = {48, 78};
  hurtbox.offset = {-24, -80};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];

  psfx.use();
}

Xander::~Xander() {
  ai.reset();
  atlas.release();
  psfx.release();
}

void Xander::setEnabled(bool value) {
  PartyMember::setEnabled(value);

  ai_goal = XanderGoals::IDLE;
  moving_x = 0;

  taking_step = false;
  step_clock = 0.0;

  target = NULL;
  tick_clock = 0;
}

void Xander::damageMorale(float magnitude) {
  PLOGI << "Xander does not possess Morale. Opting to increase his"
  << " Entropy instead.";

  increaseEntropy(magnitude);
  tp_regen_clock = 0.0;
}

void Xander::enterHitstun(DamageData &data) {
  if (nullifyHitstun(data)) {
    PLOGI << "Attempt to enter hit stun has been nullified.";
    return;
  }

  PartyMember::enterHitstun(data);
}

bool Xander::nullifyHitstun(DamageData &data) {
  if (state == CombatantState::HIT_STUN) {
    return false;
  }
  else if (data.stun_type == StunType::STAGGER) {
    return false;
  }

  if (data.damage_type == DamageType::MORALE) {
    return true;
  }
  else {
    return false;
  }
}

void Xander::setKnockback(float velocity, float seconds, 
                          Direction direction)
{
  seconds = seconds / 2;
  PLOGD << "Knockback time has been split in half.";

  PartyMember::setKnockback(velocity, seconds, direction);
}

void Xander::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  PartyMember::evaluateEvent(event);

  if (!enabled) {
    return;
  }

  bool from_itself = event->sender == this;
  switch (event->event_type) {
    case CombatantEVT::WARNING: {
      WarningCBT *warn_event = static_cast<WarningCBT*>(event.get());
      onWarning(warn_event);
      break;
    }
    case CombatantEVT::TOOK_DAMAGE: {
      TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
      bool from_mary = dmg_event->sender == player;

      if (from_mary) {
        onMaryDamageTaken(dmg_event);
      }
      else if (from_itself) {
        onDamageTaken(dmg_event);
      }

      break;
    }
    default: {
      break;
    }
  }
}

void Xander::onWarning(WarningCBT *event) {
  assert(event->sender != this);

  if (shouldAcknowledge(event)) {
    PLOGI << "Acknowledging Warning sent by Entity [ID: " 
      << event->sender->entity_id << "]";

    protect_time = event->time_until + event->active_time;
    protect_clock = 0.0;
    step_clock = 1.0;

    target = NULL;
    ai_goal = XanderGoals::PROTECT_PLR;
    psfx.play("xander_roar");
    PLOGI << "Now attempting to protect Mary.";
  }
}

bool Xander::shouldAcknowledge(WarningCBT *event) {
  if (event->assailant == NULL || team == event->assailant->team) {
    return false;
  }

  if (state != NEUTRAL) {
    return false;
  } 

  bool already_protecting = ai_goal == XanderGoals::PROTECT_PLR;
  bool on_assist = ai_goal == XanderGoals::TAIL_WHIP || 
    ai_goal == XanderGoals::METEOR;
  if (already_protecting || on_assist) {
    return false;
  }

  bool life_attack = event->action_type == ActionType::OFFENSE_HP;
  bool targeting_mary = event->target == player;
  if (!life_attack || !targeting_mary) {
    return false;
  }

  float m_max_life = player->max_life;
  Rectangle *m_hurtbox = &player->hurtbox.rect;

  bool m_at_risk = CheckCollisionRecs(*m_hurtbox, event->hitbox);
  bool potentially_fatal = player->life <= m_max_life * 0.45;
  bool not_at_risk = !CheckCollisionRecs(hurtbox.rect, event->hitbox);
  return m_at_risk && potentially_fatal && not_at_risk;
}

void Xander::onDamageTaken(TookDamageCBT *event) {
  if (event->resulting_state != HIT_STUN) {
    return;
  }

  if (event->assailant != target) {
    float retaliation_chance = ai->damaged.retaliation_chance;
    retaliation(event->assailant, retaliation_chance);
  }

  if (ai_goal != XanderGoals::TARGETING) {
    return;
  }

  float retreat_chance = ai->damaged.retreat_chance;
  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  if (percentage <= retreat_chance) {
    PLOGI << "Deciding to return to Mary's position.";
    ai_goal = XanderGoals::FOLLOW_PLR;
    step_clock = 0.5;
    acceleration = 0.5;
    psfx.play("xander_growl", 1.25);
  }
}

void Xander::onMaryDamageTaken(TookDamageCBT *event) {
  if (event->damage_type != DamageType::LIFE) {
    return;
  }

  if (event->assailant != target) {
    float retaliation_chance = ai->damaged.retaliation_chance;
    retaliation_chance += 0.25;
    retaliation(event->assailant, retaliation_chance);
  }

  PLOGI << "Deciding to return to Mary's position.";
  ai_goal = XanderGoals::FOLLOW_PLR;
  acceleration = 1.0;
  step_clock = 1.0;
  psfx.play("xander_growl", 0.75);
}

void Xander::retaliation(Combatant *assailant, float chance) {
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

  if (percentage <= chance) {
    target = assailant;
    PLOGI << "'" << name << "' [ID: " << entity_id << "] has decided to" 
    << "retaliate against: '" << target->name << "' [ID: " << 
      target->entity_id << "]";
  }
}

void Xander::behavior() {
  if (!enabled) {
    return;
  }

  if (ai_goal == XanderGoals::IDLE) {
    rootBehavior();
  }
  else if (ai_goal == XanderGoals::TARGETING) {
    targetingBehavior();
  }
}

void Xander::rootBehavior() {
  bool enemies_present = Enemy::memberCount() != 0;
  if (enemies_present) {
    chooseTarget();
  }

  if (target != NULL) {
    PLOGI << "Now targeting: '" << target->name << "' [ID: " 
      << target->entity_id << "]";
    ai_goal = XanderGoals::TARGETING;
    return;
  }

  tick_clock += Game::deltaTime();
  if (tick_clock >= 1.0) {
    ai->setGoal(ai_goal, XanderGoals::LOOK_AT_PLR, 0.20);
    tick_clock = 0;
  }

  float plr_distance = distanceTo(player);
  if (plr_distance > preferred_plr_distance) {
    ai_goal = XanderGoals::FOLLOW_PLR;
    return;
  }
}

void Xander::targetingBehavior() {
  assert(target != NULL);
  if (target->state == DEAD || !target->targetable) {
    ai_goal = XanderGoals::IDLE;
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
  ai->setGoal(ai_goal, XanderGoals::RETREATING, retreat_chance);

  if (ai_goal == XanderGoals::RETREATING) {
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

void Xander::chooseTarget() {
  if (player->state == DEAD || player->target == NULL) {
    return;
  }

  Combatant *plr_target = player->target;
  if (plr_target->state == CombatantState::DEAD) {
    return;
  }

  if (plr_target->targetable) {
    assert(team != plr_target->team);
    target = plr_target;
  }
}

void Xander::attack() {
  unique_ptr<CombatAction> action;
  action = make_unique<HandBlade>(this);
  performAction(action);
  ai->cooldown_clock = 0.0;
}

void Xander::update() {
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

      if (kb_push_back) {
        knockbackLogic();
      }
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      knockbackLogic();
      sprite = &atlas.sprites[8];
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

void Xander::neutralLogic() {
  if (ai->cooldown_clock < 1.0) {
    ai->cooldown_clock += Game::deltaTime() / ai->attack_cooldown;
  }

  float old_x = position.x;
  switch (ai_goal) {
    case XanderGoals::IDLE: {
      moving_x = 0;
      movement(speed_multiplier);
      break;
    } 
    case XanderGoals::LOOK_AT_PLR: {
      direction = directionTo(player);
      ai_goal = XanderGoals::IDLE;
      break; 
    }
    case XanderGoals::FOLLOW_PLR: {
      followPlayer();
      break;
    }
    case XanderGoals::PROTECT_PLR: {
      protectionLogic();
      break;
    }
    case XanderGoals::TARGETING: {
      targetingLogic();
      break;
    }
    case XanderGoals::RETREATING: {
      retreatingLogic();
      break;
    }
  }

  has_moved = old_x != position.x;
  animationLogic();
}

void Xander::followPlayer() {
  direction = directionTo(player);
  moving_x = direction;
  movement(speed_multiplier);

  float distance = distanceTo(player);
  if (distance <= preferred_plr_distance / 2) {
    ai_goal = XanderGoals::IDLE;
    moving_x = 0;
    taking_step = false;
    step_clock = 0.0;
  }
}

void Xander::protectionLogic() {
  Rectangle *m_rect = &player->hurtbox.rect;
  float m_area = m_rect->width * m_rect->height;

  float c_area = 0;
  if (CheckCollisionRecs(hurtbox.rect, *m_rect)) {
    Rectangle collision = GetCollisionRec(hurtbox.rect, *m_rect);
    c_area = collision.width * collision.height;
  }

  direction = directionTo(player);
  moving_x = direction;

  bool inside_hurtbox = m_area == c_area;
  if (!inside_hurtbox || taking_step) {
    acceleration = 1.0;

    movement(speed_multiplier + 0.50);
  }

  protect_clock += Game::deltaTime() / protect_time;
  if (protect_clock >= 1.0 && !taking_step) {
    ai_goal = XanderGoals::IDLE;
    moving_x = 0;
  }
}

void Xander::targetingLogic() {
  assert(target != NULL);
  direction = directionTo(target);
  moving_x = direction;

  if (ai->waiting) {
    decelerate();
    ai->waitTimer();
    return;
  }

  if (taking_step || !ai->inAttackRange(this, target)) {
    movement(speed_multiplier);
    return;
  }

  if (ai->cooldown_clock < 1.0) {
    moving_x = 0;
    return;
  }

  attack();

  float retreat_chance = ai->targeting.retreat_chance;
  ai->setGoal(ai_goal, XanderGoals::RETREATING, retreat_chance);

  if (ai_goal == XanderGoals::RETREATING) {
    PLOGI << "Retreating from target.";
    float min_retreat = ai->targeting.min_retreat;
    float max_retreat = ai->targeting.max_retreat;

    uniform_real_distribution<float> range(min_retreat, max_retreat);
    ai->retreat_time = range(Game::RNG);
  }
}

void Xander::retreatingLogic() {
  assert(target != NULL);
  if (target->state == DEAD) {
    ai_goal = XanderGoals::IDLE;
    target = NULL;
    return;
  }

  direction = directionTo(target);
  moving_x = direction * -1;
  movement(speed_multiplier);

  ai->retreat_clock += Game::deltaTime() / ai->retreat_time;
  if (taking_step || ai->retreat_clock < 1.0) {
    return;
  }

  float target_chance = ai->retreating.target_chance;
  ai->setGoal(ai_goal, XanderGoals::TARGETING, target_chance);

  if (ai_goal == XanderGoals::TARGETING) {
    float min_wait = ai->retreating.min_wait;
    float max_wait = ai->retreating.max_wait;
    ai->wait(min_wait, max_wait);
  }
  else {
    PLOGI << "Returning to idle.";
    ai_goal = XanderGoals::IDLE;
    target = NULL; 
  }

  ai->retreat_clock = 0.0;
}

void Xander::movement(float multiplier) {
  if (moving_x != 0) {
    accelerate();
  }
  else {
    decelerate();
    step_clock = 0.0;
    return;
  }

  float step_interval = getStepInterval(multiplier, true);
  if (step_interval < 0) {
    step_interval = 0.1;
  }
  
  step_clock += Game::deltaTime() / step_interval;

  if (taking_step) {
    stepping(multiplier);
    return;
  }

  if (step_clock >= 1.0) {
    takeStep();
  }
}

float Xander::getStepInterval(float multiplier, bool use_accel) {
  float factor;
  if (use_accel) {
    factor = 2.0 - (multiplier * acceleration);
  }
  else {
    factor = 2.0 - multiplier;
  }

  return def_step_interval * factor;
}

void Xander::takeStep() {
  float magnitude = def_step_distance * moving_x;
  intended_pos = Vector2Add(position, {magnitude, 0});

  animation = &anim_move;
  SpriteAnimation::progress(animation, true);

  float pitch;
  if (*animation->current == 6) {
    pitch = 1.15;
  }
  else {
    pitch = 0.80;
  }

  uniform_real_distribution<float> range(-0.15, 0.10);
  pitch = pitch + range(Game::RNG);

  float pan = 0.5;
  float distance = distanceTo(player);
  if (distance > 64) {
    float magnitude = (64 - distance) / 1000;
    int x_direction = player->directionTo(this);

    pan += magnitude * x_direction;
  }

  psfx.play("xander_footstep", pitch, pan);
  taking_step = true;
  step_clock = 0.0;
}

void Xander::stepping(float multiplier) {
  assert(taking_step);

  float speed = step_speed * multiplier;
  float magnitude = speed * Game::deltaTime();

  if (Collision::checkX(this, magnitude, moving_x)) {
    Collision::snapX(this, moving_x);
    taking_step = false;
    return;
  }

  position = Vector2MoveTowards(position, intended_pos, magnitude);
  if (FloatEquals(position.x, intended_pos.x))  {
    taking_step = false;
  }
}

void Xander::animationLogic() {
  if (has_moved) {
    rectExCorrection(bounding_box, hurtbox);
  }

  if (animation == NULL || moving_x == 0) {
    Animation *next_anim = getIdleAnim();
    SpriteAnimation::play(animation, next_anim, true);
  }

  sprite = &atlas.sprites[*animation->current];
}

Animation *Xander::getIdleAnim() {
  if (!critical_life) {
    return &anim_idle;
  }
  else {
    return &anim_crit;
  }
}


void Xander::endLogic() {
  PartyMember::endLogic();

  if (!protective && tenacity > 0) {
    priority++;
    protective = true;
    PLOGI << "'Protective' is now active.";
  }
  else if (protective && tenacity == 0) {
    priority--;
    protective = false;
    PLOGI << "'Protective' is now inactive.";
  }
}

void Xander::draw() {
  drawSprite(&atlas.sheet);
}

void Xander::drawDebug() {
  Combatant::drawDebug();
  ai->drawDebug(static_cast<int>(ai_goal), position, bounding_box);
  ai->drawDist(position, preferred_plr_distance, SKYBLUE);

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
