#pragma once
#include <string>
#include <raylib.h>
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

struct CreateStatTxtCB : CombatEvent {
  Combatant *target;
  std::string text;
  Color color;
};
