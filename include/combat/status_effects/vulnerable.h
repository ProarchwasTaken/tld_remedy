#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"
#include "data/damage.h"


/* For it's duration, Vulnerable forcefully converts any damage the 
 * afflicted sustains into Life damage. Additional modifiers may be 
 * applied depending on the initial damage type.*/
class Vulnerable : public StatusEffect {
public:
  Vulnerable(Combatant *afflicted, float seconds);
  void init(bool hide_text = false) override;

  void intercept(DamageData &data) override;
  void refresh(float seconds);
  void logic() override;

  float effect_time;
  float effect_clock = 0.0;
};
