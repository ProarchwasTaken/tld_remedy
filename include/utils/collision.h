#pragma once
#include "enums.h"
#include "base/actor.h"
#include "base/combatant.h"


namespace Collision {
  bool checkX(Actor *actor, float magnitude, int x_direction, 
              float &collision_x);
  bool checkY(Actor *actor, float magnitude, int y_direction,
              float &collision_y);

  void snapX(Actor *actor, float x, int x_direction);
  void snapY(Actor *actor, float y, int y_direction);

  int checkX(Combatant *combatant, float magnitude, Direction direction);
  void snapX(Combatant *combatant, Direction direction);
}
