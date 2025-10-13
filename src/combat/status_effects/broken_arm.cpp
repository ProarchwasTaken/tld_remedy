#include <cmath>
#include "enums.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "combat/status_effects/broken_arm.h"
#include <plog/Log.h>


BrokenArm::BrokenArm(Combatant *afflicted) : 
  StatusEffect(StatusID::BROKEN_ARM, StatusType::NEGATIVE, afflicted)
{
  name = "Broken Arm";
  persistant = true;

  float offense = afflicted->offense;
  float dec_offense = std::floorf(offense * 0.90);
  offense_lost = offense - dec_offense;
  PLOGD << "Offense to be lost: " << offense_lost;

  float intimid = afflicted->intimid;
  float dec_intimid = std::floorf(intimid * 0.90);
  intimid_lost = intimid - dec_intimid;
  PLOGD << "Intimidation to be lost: " << intimid_lost;
}

BrokenArm::~BrokenArm() {
  if (end) {
    PLOGI << "Reversing stat penalties.";
    afflicted->offense += offense_lost;
    afflicted->intimid += intimid_lost;
  }
}

void BrokenArm::init(bool hide_text) {
  PLOGI << "Decreasing afflicted's offense and intimidation by 10%";
  afflicted->offense -= offense_lost;
  afflicted->intimid -= intimid_lost;

  PLOGD << "Result: {OFF: " << afflicted->offense << ", INT: " <<
  afflicted->intimid << "}";

  StatusEffect::init(hide_text);
}
