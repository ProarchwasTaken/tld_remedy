#include <memory>
#include "base/combatant.h"
#include "data/combatant_event.h"
#include "base/party_member.h"
#include "base/status_effect.h"


/* A positive status effect that gradually restores a percentage of the
 * afflicted's Life. The speed of which varies depending on the 
 * party member's recovery. If the afflicted sustains Life damage,
 * the effect will end prematurely.*/
class Mending : public StatusEffect {
public:
  Mending(PartyMember *afflicted, float percentage, float speed);
  ~Mending();

  void refresh(float percentage, float speed);

  void init(bool hide_text = false) override;
  void evaluateEvent(std::unique_ptr<CombatantEvent> &event) override;
  void logic() override;
private:
  PartyMember *afflicted;
  float to_be_healed;
  float speed;
};
