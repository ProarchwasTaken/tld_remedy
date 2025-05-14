#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"


struct MapTransData {
  std::string map_dest;
  std::string spawn_dest;
  Rectangle rect;
  Direction direction;
};
