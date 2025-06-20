#pragma once
#include "base/combatant.h"
#include "data/keybinds.h"
#include "data/session.h"


class PlayerCombatant : public Combatant {
public:
  PlayerCombatant(Player *data);
  static void setControllable(bool value);
  static CombatKeybinds key_bind;

  void behavior() override;
  void movementInput(bool gamepad);
  void actionInput(bool gamepad);

  void update() override;
  void neutralLogic();
  void movement();

  void draw() override;

  float morale;
  float init_morale;
  float max_morale;
private:
  const float default_speed = 1.2;
  bool has_moved = false;
  bool moving = true;
  int moving_x = 0;

  static bool controllable;
};
