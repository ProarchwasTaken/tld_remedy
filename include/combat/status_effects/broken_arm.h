#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Lowers the afflicted's Offense and Intimidation. Effectively
 * hindering the Combatant's damage potential. The amount decreased
 * is determined by the combatant's Resilience. If Resilience is high
 * enough, the negative effects will virtually be nullified.*/
class BrokenArm : public StatusEffect {
public:
  BrokenArm(Combatant *afflicted);
  ~BrokenArm();

  void init(bool hide_text = false) override;
  void applyPenalty();
  void negateEffect();

  bool negated = false;
private:
  int offense_lost = 0;
  int intimid_lost = 0;
};
