#pragma once
#include <string>
#include "enums.h"

struct FieldEvent {
  FieldEventType event_type;
};

struct LoadMapEvent : FieldEvent {
  std::string map_name;
  std::string spawn_point;
};
