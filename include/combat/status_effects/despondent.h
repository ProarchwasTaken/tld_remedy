#pragma once
#include "base/party_member.h"
#include "base/status_effect.h"


class Despondent : public StatusEffect {
public:
  Despondent(PartyMember *afflicted);

  void logic() override;
private:
  PartyMember *afflicted;
  
  float delay_clock = 0.0;
  float delay_time = 3;
};
