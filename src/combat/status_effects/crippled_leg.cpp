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

  float speed = afflicted->speed_multiplier;
  float dec_speed = speed * 0.85;
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
  PLOGI << "Decreasing afflicted's speed multiplier by 15%";
  afflicted->speed_multiplier -= speed_lost;
  negated = false;
}

void CrippledLeg::negateEffect() {
  PLOGI << "Reversing stat penalties.";
  afflicted->speed_multiplier += speed_lost;
  negated = true;
}
