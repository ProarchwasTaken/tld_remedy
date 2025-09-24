#pragma once
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"


/* Decreases a PartyMember's defense and recovery by 50%. Making the 
 * Combatant a lot more easy to kill.*/
class Mangled : public StatusEffect {
public:
  Mangled(PartyMember *afflicted);
  ~Mangled();

  void init(bool hide_text = false) override;
private:
  PartyMember *afflicted;

  int defense_lost = 0;
  float recovery_lost = 0;
};
