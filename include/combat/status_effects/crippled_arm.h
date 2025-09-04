#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Lowers the afflicted's Offense and Intimidation by 10%. Effectively
 * hindering the Combatant's damage potential.*/
class CrippledArm : public StatusEffect {
public:
  CrippledArm(Combatant *afflicted);
  ~CrippledArm();

  void init() override;
private:
  int offense_lost = 0;
  int intimid_lost = 0;
};
