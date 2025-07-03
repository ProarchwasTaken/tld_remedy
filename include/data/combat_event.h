#pragma once
#include "base/combatant.h"
#include "enums.h"


struct CombatEvent {
  CombatEVT event_type;
};

struct DeleteEntityCB : CombatEvent {
  int entity_id;
};

struct CreateDmgNumCB : CombatEvent {
  Combatant *target;
  DamageType damage_type;
  float damage_taken;
};
