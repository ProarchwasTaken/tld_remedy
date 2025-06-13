#pragma once
#include "base/combatant.h"
#include "data/session.h"


class PlayerCombatant : public Combatant {
public:
  PlayerCombatant(Player *data);

  void behavior() override;
  void update() override;
  void draw() override;

  float morale;
  float max_morale;
private:
  float init_morale;
};
