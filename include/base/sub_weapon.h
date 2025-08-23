#pragma once
#include <string>
#include <memory>
#include "enums.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"


class SubWeapon {
public:
  SubWeapon(std::string name, SubWeaponID id, Mary *user);
  ~SubWeapon() = default;

  bool lightTechCondition();
  bool heavyTechCondition();
  virtual std::unique_ptr<CombatAction> lightTechnique();

  virtual std::unique_ptr<CombatAction> heavyTechnique();

  std::string name;
  SubWeaponID id;

  float tech1_cost;
  TechCostType tech1_type;

  float tech2_cost;
  TechCostType tech2_type;
protected:
  Mary *user;
};
