#pragma once
#include "base/combatant.h"
#include "base/combat_action.h"


class Attack : public CombatAction {
public:
  Attack(Combatant *user);

  void windUp() override {};
  void action() override {};
  void endLag() override {};
};
