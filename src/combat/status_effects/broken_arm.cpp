#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "combat/status_effects/broken_arm.h"
#include <plog/Log.h>


BrokenArm::BrokenArm(Combatant *afflicted) : 
  StatusEffect(StatusID::BROKEN_ARM, StatusType::NEGATIVE, afflicted)
{
  name = "Broken Arm";
  persistent = true;

  float percentage = Lerp(0.50, 1.0, afflicted->resilience);
  PLOGD << "Percentage Reduction: " << 1.0 - percentage;

  if (percentage >= 1.0) {
    PLOGD << "Effect has been nullied due to the Afflicted's Resilience.";
    return;
  }

  float offense = afflicted->offense;
  float dec_offense = std::floorf(offense * percentage);
  offense_lost = offense - dec_offense;
  PLOGD << "Offense to be lost: " << offense_lost;

  float intimid = afflicted->intimid;
  float dec_intimid = std::floorf(intimid * percentage);
  intimid_lost = intimid - dec_intimid;
  PLOGD << "Intimidation to be lost: " << intimid_lost;
}

BrokenArm::~BrokenArm() {
  if (end && !negated) {
    negateEffect();
  }
}

void BrokenArm::init(bool hide_text) {
  applyPenalty();

  PLOGD << "Result: {OFF: " << afflicted->offense << ", INT: " <<
  afflicted->intimid << "}";

  StatusEffect::init(hide_text);
}

void BrokenArm::applyPenalty() {
  PLOGI << "Decreasing afflicted's offense and intimidation.";
  afflicted->offense -= offense_lost;
  afflicted->intimid -= intimid_lost;
  negated = false;
}

void BrokenArm::negateEffect() {
  PLOGI << "Reversing stat penalties.";
  afflicted->offense += offense_lost;
  afflicted->intimid += intimid_lost;
  negated = true;
}
