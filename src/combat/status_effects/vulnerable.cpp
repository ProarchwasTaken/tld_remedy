#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "data/damage.h"
#include "combat/status_effects/vulnerable.h"
#include <plog/Log.h>


Vulnerable::Vulnerable(Combatant *afflicted, float seconds) : 
  StatusEffect(StatusID::VULNERABLE, StatusType::NEGATIVE, afflicted)
{
  name = "Vulnerable";
  persistent = false;
  this->effect_time = seconds;
}

void Vulnerable::init(bool hide_text) {
  PLOGI << "The afflicted is now vulnerable for " << effect_time << "s.";
  StatusEffect::init(hide_text);
}

void Vulnerable::intercept(DamageData &data) {
  DamageType initial_type = data.damage_type;
  if (initial_type == DamageType::MORALE) {
    data.power -= 0.25;
    data.ent_split = 0.5;
  }
  else {
    data.power += 0.25;
    data.stun_time = data.stun_time * 1.20;

    data.stun_type = StunType::STAGGER;
    Combatant::sfx.play("damage_stagger");
  }

  data.damage_type = DamageType::LIFE;
  PLOGI << "Damage type has been forcefully set to LIFE";
}

void Vulnerable::refresh(float seconds) {
  if (seconds > effect_time) {
    PLOGD << "Extending the effect's duration to: " << seconds << "s.";
    effect_time = seconds;
  }

  effect_clock = 0.0;
  PLOGI << "Effect has been refreshed.";
}

void Vulnerable::logic() {
  effect_clock += Game::deltaTime() / effect_time;
  if (effect_clock >= 1.0) {
    end = true;
  }
}

