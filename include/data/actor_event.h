#pragma once
#include "enums.h"
#include "base/entity.h"


struct ActorEvent {
  Entity *sender;
  ActorEventType event_type;
};


