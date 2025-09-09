#pragma once
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"


struct CombatantEvent {
  Entity *sender;
  CombatantEVT event_type;
};

struct TookDamageCBT : CombatantEvent {
  float damage_taken;
  DamageType damage_type;
  CombatantState resulting_state;

  float stun_time;
  StunType stun_type;

  Combatant *assailant;
};
