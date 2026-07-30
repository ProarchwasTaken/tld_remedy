#pragma once
#include <cstddef>
#include "base/combatant.h"


struct DamageData {
  DamageType damage_type;
  DamageType calculation;

  float stun_time;
  StunType stun_type;

  float dmg_floor = 1; 
  float knockback = 0;
  float hit_stop = 0;

  Combatant *assailant = NULL;
  Rectangle *hitbox = NULL;
  int *a_atk = NULL;
  int *b_def = NULL;

  float power = 1.0;
  float ent_split = 0.0;
  bool force_hitstop = false;
  bool intercepted = false;
};
