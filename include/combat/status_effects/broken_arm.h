#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Lowers the afflicted's Offense and Intimidation by 10%. Effectively
 * hindering the Combatant's damage potential.*/
class BrokenArm : public StatusEffect {
public:
  BrokenArm(Combatant *afflicted);
  ~BrokenArm();

  void init(bool hide_text = false) override;
  void applyPenalty();
  void negateEffect();
private:
  bool negated = false;

  int offense_lost = 0;
  int intimid_lost = 0;
};
