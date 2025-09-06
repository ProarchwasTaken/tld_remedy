#include <cmath>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/damage.h"
#include "combat/status_effects/despondent.h"
#include <plog/Log.h>


Despondent::Despondent(PartyMember *afflicted) : 
  StatusEffect(StatusID::DESPONDENT, StatusType::NEGATIVE, afflicted)
{
  name = "Despondent";
  persistant = false;
  this->afflicted = afflicted;

  float intimid = afflicted->intimid;
  float dec_intimid = std::floorf(intimid * 0.90);
  intimid_lost = intimid - dec_intimid;
  PLOGD << "Intimidation to be lost: " << intimid_lost;
}

void Despondent::init() {
  afflicted->demoralized = true;

  PLOGI << "Decreasing afflicted's intimidation by 10%";
  afflicted->intimid -= intimid_lost;

  PLOGD << "Result: " << afflicted->intimid;

  StatusEffect::init();
}

Despondent::~Despondent() {
  if (end) {
    PLOGI << "Reversing stat penalties.";
    afflicted->demoralized = false;
    afflicted->intimid += intimid_lost;

    afflicted->sfx.play("despondent_recover");
  }
}

void Despondent::intercept(DamageData &data) {
  if (afflicted->state != CombatantState::NEUTRAL) {
    PLOGI << "Afflicted has taken damage while not in Neutral.";
    data.stun_type = StunType::STAGGER;
    data.calculation = DamageType::LIFE;
    data.damage_type = DamageType::LIFE;

    Combatant::sfx.play("damage_stagger");
    PLOGD << "Forcing Life damage calculation and Stagger.";
  }
}

void Despondent::logic() {
  if (delay_clock <= 1.0) {
    delay_clock += Game::deltaTime() / delay_time;
    return;
  }

  float recovery = afflicted->recovery;
  float magnitude = (afflicted->max_morale * 0.035) * recovery;
  afflicted->morale += magnitude * Game::deltaTime();

  float init_morale = afflicted->init_morale;
  if (afflicted->morale >= init_morale) {
    end = true;
  }
}
