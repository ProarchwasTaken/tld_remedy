#include <cassert>
#include "enums.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/xander.h"
#include <plog/Log.h>

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

  tenacity = 3;
  tp_natural = 0.10;
  tp_threshold = tp_natural;
  tp_regen_delay = 10.0;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;
  dexterity = data->dexterity;
  discipline = data->discipline;

  recovery = data->recovery;
  resilience = data->resilience;

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
  atlas.release();
}

void Xander::damageMorale(float magnitude) {
  PLOGI << "Xander does not possess Morale. Opting to increase his"
  << " Entropy instead.";

  increaseEntropy(magnitude);
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

void Xander::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

      animationLogic();
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
