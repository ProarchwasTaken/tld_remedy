#pragma once
#include <memory>
#include "base/sub_weapon.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"


class Knife : public SubWeapon {
public:
  Knife(Mary *user);

  std::unique_ptr<CombatAction> lightTechnique() override;
  std::unique_ptr<CombatAction> heavyTechnique() override;
};
