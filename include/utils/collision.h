#pragma once
#include "base/actor.h"


namespace Collision {
  bool checkX(Actor *actor, float magnitude, int x_direction, 
              float &collision_x);
  bool checkY(Actor *actor, float magnitude, int y_direction,
              float &collision_y);
}
