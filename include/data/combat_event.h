#pragma once
#include "data/damage.h"
#include "enums.h"


struct CombatEvent {
  CombatEVT event_type;
};

struct DeleteEntityCB : CombatEvent {
  int entity_id;
};

struct CreateDmgNumCB : CombatEvent {
  Vector2 position;
  DamageData data;
};
