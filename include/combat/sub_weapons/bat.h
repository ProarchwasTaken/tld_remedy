#pragma once
#include <memory>
#include "base/sub_weapon.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"


class Bat : public SubWeapon {
public:
  Bat(Mary *user);
  ~Bat();

  std::unique_ptr<CombatAction> lightTechnique() override;
};
