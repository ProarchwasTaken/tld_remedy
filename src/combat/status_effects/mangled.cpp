#include <cmath>
#include "enums.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "combat/status_effects/mangled.h"
#include <plog/Log.h>


Mangled::Mangled(Combatant *afflicted) : 
  StatusEffect(StatusID::MANGLED, StatusType::NEGATIVE, afflicted)
{
  name = "Mangled";
  persistent = true;

  float defense = afflicted->defense;
  float dec_defense = std::floorf(defense * 0.50);
  defense_lost = defense - dec_defense;
  PLOGD << "Defense to be lost: " << defense_lost;

  float recovery = afflicted->recovery;
  float dec_recovery = recovery * 0.50;
  recovery_lost = recovery - dec_recovery;
  PLOGD << "Recovery to be lost: " << recovery_lost;

  PLOGD << "Resilience to be lost: " << resilience_lost;
}

Mangled::~Mangled() {
  if (end && !negated) {
    negateEffect();
  }
}

void Mangled::init(bool hide_text) {
  applyPenalty();

  PLOGD << "Result: {DEF: " << afflicted->defense << ", REC: " <<
  afflicted->recovery << ", RES: " << afflicted->resilience << "}";

  StatusEffect::init(hide_text);
}

void Mangled::applyPenalty() {
  PLOGI << "Decreasing the afflicted's defense and recovery by 50%";
  afflicted->defense -= defense_lost;
  afflicted->recovery -= recovery_lost;
  afflicted->resilience -= resilience_lost;
  negated = false;
}

void Mangled::negateEffect() {
  PLOGI << "Reversing stat penalties.";
  afflicted->defense += defense_lost;
  afflicted->recovery += recovery_lost;
  afflicted->resilience += resilience_lost;
  negated = true;
}
