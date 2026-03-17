#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "combat/status_effects/crippled_leg.h"
#include <plog/Log.h>


CrippledLeg::CrippledLeg(Combatant *afflicted) : 
  StatusEffect(StatusID::CRIPPLED_LEG, StatusType::NEGATIVE, afflicted)
{
  name = "Crippled Leg";
  persistent = true;

  float percentage = Lerp(0.25, 1.0, afflicted->resilience);
  PLOGD << "Percentage Reduction: " << 1.0 - percentage;

  if (percentage >= 1.0) {
    PLOGD << "Effect has been nullied due to the Afflicted's Resilience.";
    return;
  }

  float speed = afflicted->speed_multiplier;
  float dec_speed = speed * percentage;
  speed_lost = speed - dec_speed;
  PLOGD << "Speed to be lost: " << speed_lost;
}

CrippledLeg::~CrippledLeg() {
  if (end && !negated) {
    negateEffect();
  }
}

void CrippledLeg::init(bool hide_text) {
  applyPenalty();
  PLOGD << "Result: " << afflicted->speed_multiplier;

  StatusEffect::init(hide_text);
}

void CrippledLeg::applyPenalty() {
  PLOGI << "Decreasing afflicted's speed multiplier.";
  afflicted->speed_multiplier -= speed_lost;
  negated = false;
}

void CrippledLeg::negateEffect() {
  PLOGI << "Reversing stat penalties.";
  afflicted->speed_multiplier += speed_lost;
  negated = true;
}
