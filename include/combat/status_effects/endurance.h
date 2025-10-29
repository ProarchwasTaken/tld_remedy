#include "base/party_member.h"
#include "base/status_effect.h"


class Endurance : public StatusEffect {
public:
  Endurance(PartyMember *afflicted);
  ~Endurance();

  void refresh();

  void init(bool hide_text = false) override;
  void logic() override;
private:
  PartyMember *afflicted;

  float resilience_gained = 0.20;
  float tenacity_given;

  float delay = 2.0;
  float clock = 0.0;
};
