#pragma once
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"


class Attack : public CombatAction {
public:
  Attack(Combatant *user, RectEx hitbox);

  void windUp() override {};
  void action() override;
  void endLag() override {};

  void drawDebug() override;
private:
  RectEx hitbox;

  bool attack_connected = false;
  DamageData data;
};
