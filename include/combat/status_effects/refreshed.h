#pragma once
#include "base/party_member.h"
#include "base/status_effect.h"


class Refreshed : public StatusEffect {
public:
  Refreshed(PartyMember *afflicted);

  void init(bool hide_text = false) override;
  ~Refreshed();

  void refresh();

  void logic() override;
private:
  PartyMember *afflicted;

  float speed_gained = 0.0;
  float rec_gained = 0.0;

  float effect_clock = 0.0;
  float effect_time = 20.0;
};
