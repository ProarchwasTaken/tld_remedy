#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"


/* The PartyMember subclass is used by Combatants who are on the player's
 * side. Including the Combatant that's directly controlled by them.*/
class PartyMember : public Combatant {
public:
  static int memberCount() {return member_count;}
  PartyMember(std::string name, PartyMemberID id, Vector2 position);
  ~PartyMember();

  void takeDamage(DamageData &data) override;

  void damageMorale(float magnitude) override;
  void increaseMorale(float magnitude) override;

  PartyMemberID id;
  bool important = false;

  float morale;
  float init_morale;
  float max_morale;
private:
  static int member_count;
};
