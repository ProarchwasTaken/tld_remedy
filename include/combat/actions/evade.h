#pragma once
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"


class Evade : public CombatAction {
public:
  Evade(PartyMember *user, RectEx hitbox);
  ~Evade();

  void intercept(DamageData &data) override;

  void windUp() override;
  void action() override;
  void endLag() override;

  void drawDebug() override;

  bool evaded_attack = false;
private:
  PartyMember *user;
  RectEx hitbox;
};
