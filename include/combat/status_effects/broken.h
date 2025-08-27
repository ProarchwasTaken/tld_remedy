#pragma once
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"


/* Decreases a PartyMember's defense and recovery by 50%. Making the 
 * Combatant a lot more easy to kill.*/
class Broken : public StatusEffect {
public:
  Broken(PartyMember *afflicted);
  ~Broken();

  void init() override;
private:
  PartyMember *afflicted;

  int defense_lost = 0;
  float recovery_lost = 0;
};
