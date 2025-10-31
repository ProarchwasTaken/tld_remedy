#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Decreases a PartyMember's defense and recovery by 50%. Making the 
 * Combatant a lot more easy to kill.*/
class Mangled : public StatusEffect {
public:
  Mangled(Combatant *afflicted);
  ~Mangled();

  void init(bool hide_text = false) override;
  void applyPenalty();
  void negateEffect();

  bool negated = false;
private:
  int defense_lost = 0;
  float recovery_lost = 0;
  float resilience_lost = 0.20;
};
