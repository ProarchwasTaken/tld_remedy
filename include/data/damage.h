#pragma once
#include <cstddef>
#include "base/combatant.h"
#include "enums.h"


struct DamageData {
  DamageType damage_type;
  DamageType calulation;

  float stun_time;
  StunType stun_type;

  float base_damage = 0; 
  float knockback = 0;
  float hit_stop = 0;

  Combatant *assailant = NULL;
  int *a_atk = NULL;
  int *b_def = NULL;
  float atk_mod = 1.20;
  float def_mod = 0.80;
};
