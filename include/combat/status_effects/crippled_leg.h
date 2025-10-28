#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Decreases the afflicted's speed_multiplier by 25%. Basically crippling
 * the Combatant's mobility and combo potential.*/
class CrippledLeg : public StatusEffect {
public:
  CrippledLeg(Combatant *afflict);
  ~CrippledLeg();

  void init(bool hide_text = false) override;
  void applyPenalty();
  void negateEffect();
private:
  bool negated = false;
  float speed_lost = 0;
};
