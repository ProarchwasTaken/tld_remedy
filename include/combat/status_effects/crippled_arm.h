#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


class CrippledArm : public StatusEffect {
public:
  CrippledArm(Combatant *afflicted);
  ~CrippledArm();

  void init() override;
private:
  int offense_lost = 0;
  int intimid_lost = 0;
};
