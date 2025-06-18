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
private:
  float min_length = 1.0;
  float min_speed = 0.30;
  float fraction_speed = 0.05;
};
