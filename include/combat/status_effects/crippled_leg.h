#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Decreases the afflicted's Speed Multiplier. Basically crippling the 
 * Combatant's mobility and combo potential. The amount decreased is 
 * determined by the combatant's Resilience. If Resilience is high
 * enough, the negative effects will virtually be nullified.*/
class CrippledLeg : public StatusEffect {
public:
  CrippledLeg(Combatant *afflict);
  ~CrippledLeg();

  void init(bool hide_text = false) override;
  void applyPenalty();
  void negateEffect();

  bool negated = false;
private:
  float speed_lost = 0;
};
