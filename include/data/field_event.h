#pragma once
#include <string>
#include "enums.h"

struct FieldEvent {
  FieldEVT event_type;
};

struct LoadMapEvent : FieldEvent {
  std::string map_name;
  std::string spawn_point;
};

struct UpdateCommonEvent : FieldEvent {
  int object_id;
  bool active;
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
