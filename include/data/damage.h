#pragma once
#include <cstddef>
#include "base/combatant.h"
#include "enums.h"


struct DamageData {
  float base_damage; 
  DamageType damage_type;
  DamageType calulation;

  float stun_time;
  StunType stun_type;

  float knockback;

  Combatant *assailant = NULL;
  int *a_atk = NULL;
  int *b_def = NULL;
};
