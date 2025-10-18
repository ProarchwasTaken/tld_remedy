#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/damage.h"


class Mending : public StatusEffect {
public:
  Mending(PartyMember *afflicted, float percentage, float speed);

  void intercept(DamageData &data) override;
  void logic() override;
private:
  PartyMember *afflicted;
  float to_be_healed;
  float speed;
};
