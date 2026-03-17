#pragma once
#include "base/combatant.h"
#include "base/status_effect.h"


/* Decreases a PartyMember's Defense and Recovery by 50%. Making the 
 * Combatant a lot more easy to kill. Resilience is also hard decreased by 
 * 20%. Which raises the severity of all status ailments inflicted 
 * afterwards. 
 *
 * The amount of which Defense and Recovery are reduced is determined by
 * the afflicted's Resilience. However the effect cannot be nullified 
 * entirely.*/
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
