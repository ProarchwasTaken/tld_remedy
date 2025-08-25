#pragma once
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "combat/combatants/party/mary.h"


class KnifeCleave : public CombatAction {
public:
  KnifeCleave(Mary *user);

  void windUp() override;
  void action() override;
  void endLag() override;

  void drawDebug() override;
private:
  RectEx hitbox;
};
