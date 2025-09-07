#pragma once
#include <string>
#include <memory>
#include "enums.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"


/* Sub-Weapons function as extensions to the Player's base moveset.
 * It's purpose is to provide the player it weapon techniques that's
 * associated with whatever Sub-Weapon they're using, along with 
 * functions for checking if said techniques could be used depending
 * on cost and type.*/
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

  std::string tech1_name;
  float tech1_cost;
  TechCostType tech1_type;

  std::string tech2_name;
  float tech2_cost;
  TechCostType tech2_type;
protected:
  Mary *user;
};
