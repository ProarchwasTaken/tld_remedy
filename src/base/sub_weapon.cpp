#include <string>
#include <memory>
#include "enums.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"
#include "base/sub_weapon.h"
#include <plog/Log.h>

using std::string, std::unique_ptr;


SubWeapon::SubWeapon(string name, SubWeaponID id, Mary *user) {
  this->name = name;
  this->id = id;
  this->user = user;
  PLOGI << "Sub-Weapon: '" << name << "' has been assigned to the"
    " Player Combatant.";
}

std::unique_ptr<CombatAction> SubWeapon::lightTechnique() {
  PLOGI << "Light Technique is not available.";
  return nullptr;
}

std::unique_ptr<CombatAction> SubWeapon::heavyTechnique() {
  PLOGI << "Heavy Technique is not available.";
  return nullptr;
}
