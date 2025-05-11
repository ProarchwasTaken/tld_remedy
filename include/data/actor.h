#pragma once
#include <raylib.h>
#include "enums.h"


struct ActorData {
  Vector2 position;
  Direction direction;
  ActorType type;
};
