#include <cmath>
#include "enums.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "combat/status_effects/broken.h"
#include <plog/Log.h>


Broken::Broken(PartyMember *afflicted) : 
  StatusEffect(StatusID::BROKEN, StatusType::NEGATIVE, afflicted)
{
  name = "Broken";
  persistant = true;
  this->afflicted = afflicted;

  float defense = afflicted->defense;
  float dec_defense = std::floorf(defense * 0.50);
  defense_lost = defense - dec_defense;
  PLOGD << "Defense to be lost: " << defense_lost;

  float recovery = afflicted->recovery;
  float dec_recovery = recovery * 0.50;
  recovery_lost = recovery - dec_recovery;
  PLOGD << "Recovery to be lost: " << recovery_lost;
}

Broken::~Broken() {
  if (end) {
    PLOGI << "Reversing stat penalties.";
    afflicted->defense += defense_lost;
    afflicted->recovery += recovery_lost;
  }
}

void Broken::init(bool hide_text) {
  PLOGI << "Decreasing the afflicted's defense and recovery by 50%";
  afflicted->defense -= defense_lost;
  afflicted->recovery -= recovery_lost;

  PLOGD << "Result: {DEF: " << afflicted->defense << ", REC: " <<
  afflicted->recovery << "}";

  StatusEffect::init(hide_text);
}
