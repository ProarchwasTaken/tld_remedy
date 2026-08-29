#include <cassert>
#include <memory>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/xander.h"
#include <plog/Log.h>

using  std::make_unique;
SpriteAtlas Xander::atlas("combatants", "xander_combatant");


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

  tenacity = 4.5;
  tp_natural = 0.15;
  tp_threshold = tp_natural;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;
  dexterity = data->dexterity;
  discipline = data->discipline;

  recovery = data->recovery;
  resilience = data->resilience;

  ai = make_unique<XanderAI>();

  tech1 = {"Tail Whip", TechCostType::LIFE, 2.0};
  tech1.cooldown = 5.0;

  tech2 = {"Steel Wall", TechCostType::LIFE, 0.0};
  tech2.cooldown = 8.0;

  afflictPersistent(data->status);

  bounding_box.scale = {144, 128};
  bounding_box.offset = {-72, -128};
  hurtbox.scale = {48, 78};
  hurtbox.offset = {-24, -80};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];
}

Xander::~Xander() {
  ai.reset();
  atlas.release();
}

void Xander::setEnabled(bool value) {
  PartyMember::setEnabled(value);

  ai_goal = XanderGoals::IDLE;
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

void Xander::behavior() {
  if (!enabled) {
    return;
  }

  if (ai_goal == XanderGoals::IDLE) {
    rootBehavior();
  }
}

void Xander::rootBehavior() {
  tick_clock += Game::deltaTime();
  if (tick_clock >= 1.0) {
    ai->setGoal(ai_goal, XanderGoals::LOOK_AT_PLR, 0.75);
    tick_clock = 0;
  }
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

      // This is temporary!! Remove this later!!
      sprite = &atlas.sprites[4];
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();
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
    case XanderGoals::IDLE: 
      break;
    case XanderGoals::LOOK_AT_PLR:
      direction = directionTo(player);
      ai_goal = XanderGoals::IDLE;
      break; 
  }

  animationLogic();
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

void Xander::animationLogic() {
  Animation *next_anim = getIdleAnim();
  SpriteAnimation::play(animation, next_anim, true);
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

void Xander::draw() {
  drawSprite(&atlas.sheet);
}

void Xander::drawDebug() {
  Combatant::drawDebug();
  ai->drawDebug(static_cast<int>(ai_goal), position, bounding_box);

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
