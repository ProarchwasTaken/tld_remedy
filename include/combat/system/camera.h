#pragma once
#include <raylib.h>
#include "combat/combatants/player.h"


class CombatCamera : public Camera2D {
public:
  CombatCamera();

  void update(PlayerCombatant *player);
  void enemyTargeting(PlayerCombatant *player);
  void follow(float x);

  static Rectangle area;
};
