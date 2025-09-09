#pragma once
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"


struct CombatantEvent {
  Entity *sender;
  CombatantEVT event_type;
};

struct TookDamageEvent {
  float damage_taken;
  DamageType damage_type;
  StunType stun_type;

  float stun_time;
  Combatant *assailant;
};
