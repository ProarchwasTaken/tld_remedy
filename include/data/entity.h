#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"


struct EntityData {
  EntityType type;
};

struct MapTransData : EntityData {
  std::string map_dest;
  std::string spawn_dest;
  Rectangle rect;
  Direction direction;
};

struct PickupData : EntityData {
  Vector2 position;
  PickupType pickup_type;
  int count;
};
