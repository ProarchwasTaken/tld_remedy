#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/damage.h"


class Mending : public StatusEffect {
public:
  Mending(PartyMember *afflicted, float percentage, float speed);
  ~Mending();

  void init(bool hide_text = false) override;
  void intercept(DamageData &data) override;
  void logic() override;
private:
  PartyMember *afflicted;
  float to_be_healed;
  float speed;
};
