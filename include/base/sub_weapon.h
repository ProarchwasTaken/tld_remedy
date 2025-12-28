#pragma once
#include <cstddef>
#include <string>
#include <memory>
#include "enums.h"
#include "base/combat_action.h"
#include "data/technique.h"
#include "combat/combatants/party/mary.h"


/* Sub-Weapons function as extensions to the Player's base moveset.
 * It's purpose is to provide the player it weapon techniques that's
 * associated with whatever Sub-Weapon they're using, along with 
 * functions for checking if said techniques could be used depending
 * on cost and type.*/
class SubWeapon {
public:
  SubWeapon(std::string name, SubWeaponID id, Mary *user);
  virtual ~SubWeapon() = default;

  bool lightTechCondition();
  bool heavyTechCondition();
  virtual std::unique_ptr<CombatAction> lightTechnique();

  virtual std::unique_ptr<CombatAction> heavyTechnique();

  std::string name;
  SubWeaponID id;

  Technique *tech1 = NULL;
  Technique *tech2 = NULL;
protected:
  Mary *user;
};
