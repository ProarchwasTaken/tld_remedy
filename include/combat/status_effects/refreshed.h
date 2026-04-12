#pragma once
#include "base/party_member.h"
#include "base/status_effect.h"


/* Boosts a Combatant's dexterity, speed multiplier, and recovery for a 
 * certain amount of time. While the effect is active, the afflicted's 
 * Morale will also regenerate at a rate that's dependent on their recovery.*/
class Refreshed : public StatusEffect {
public:
  Refreshed(PartyMember *afflicted);

  void init(bool hide_text = false) override;
  ~Refreshed();

  void refresh();

  void logic() override;
private:
  PartyMember *afflicted;

  int dex_gained = 0.0;
  float speed_gained = 0.0;
  float rec_gained = 0.0;

  float effect_clock = 0.0;
  float effect_time = 20.0;
};
