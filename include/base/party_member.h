#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"


class PartyMember : public Combatant {
public:
  static int memberCount() {return member_count;}
  PartyMember(std::string name, PartyMemberID id, Vector2 position);
  ~PartyMember();

  void damageMorale(float magnitude) override;
  void increaseMorale(float magnitude) override;

  PartyMemberID id;

  float morale;
  float init_morale;
  float max_morale;
private:
  static int member_count;
};
