#pragma once
#include "base/combatant.h"
#include "data/session.h"


class PlayerCombatant : public Combatant {
public:
  PlayerCombatant(Player *data);
  static void setControllable(bool value);

  void behavior() override;
  void movementInput(bool gamepad);

  void update() override;
  void movement();

  void draw() override;

  float morale;
  float max_morale;
private:
  float init_morale;

  const float default_speed = 1.75;
  bool has_moved = false;
  int moving_x = 0;

  static bool controllable;
};
