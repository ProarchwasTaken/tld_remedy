#pragma once
#include "enums.h"
#include "base/entity.h"


struct ActorEvent {
  Entity *sender;
  ActorEventType event_type;
};

struct InteractEvent : ActorEvent {
  int target_id;
};

