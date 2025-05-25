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

struct DeleteEntityEvent : FieldEvent {
  int entity_id;
};

struct SetSuppliesEvent : FieldEvent {
  int value;
};

struct AddSuppliesEvent : FieldEvent {
  int magnitude;
};

struct SetPlrLifeEvent : FieldEvent {
  float value;
};
