#pragma once
#include "base/party_member.h"
#include "data/damage.h"
#include "base/status_effect.h"


/* Despondent is a status effect that make the afflicted very vulnerable
 * through many different ways. Only PartyMembers can recieve this
 * ailment, and it's when their Morale goes below zero.*/
class Despondent : public StatusEffect {
public:
  Despondent(PartyMember *afflicted);

  void init(bool hide_text = false) override;
  ~Despondent();

  void intercept(DamageData &data) override;
  void logic() override;
private:
  PartyMember *afflicted;

  int intimid_lost = 0;
  
  float delay_clock = 0.0;
  float delay_time = 5;
};
