#include <memory>
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/combatant_event.h"


/* While active, Endurance negates the effects of any negative status 
 * ailments the afflicted may have. Particularly persistent effects like
 * Broken Arm, Crippled Leg, and Mangled. The afflicted is also given
 * Tenacity that will decay overtime. The effect will wear off when
 * that tenacity fully decays.*/
class Endurance : public StatusEffect {
public:
  Endurance(PartyMember *afflicted);
  ~Endurance();

  void refresh();

  void init(bool hide_text = false) override;
  void evaluateEvent(std::unique_ptr<CombatantEvent> &event) override;

  void negateAilment(StatusEffect *effect);
  void enableAilment(StatusEffect *effect);
  void logic() override;
private:
  PartyMember *afflicted;

  float resilience_gained = 0.20;
  float tenacity_given;

  float delay = 3.0;
  float clock = 0.0;
};
