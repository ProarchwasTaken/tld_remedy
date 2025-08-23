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

bool SubWeapon::lightTechCondition() {
  if (user->demoralized) {
    PLOGI << "Player is Despondent. Unable to use Technique!";
    return false;
  }

  bool using_mp = tech1_type == TechCostType::MORALE;
  if (using_mp && user->morale >= tech1_cost) {
    user->morale -= tech1_cost;
    PLOGI << "Decreased Morale to: " << user->morale;
    return true;
  }
  else if (user->life >= tech1_cost) {
    user->increaseExhaustion(tech1_cost);
    return true;
  }
  else {
    PLOGI << "Player does not meet conditions to use Light Tech.";
    return false;
  }
}

bool SubWeapon::heavyTechCondition() {
  if (user->demoralized) {
    PLOGI << "Player is Despondent. Unable to use Technique!";
    return false;
  }

  bool using_mp = tech2_type == TechCostType::MORALE;
  if (using_mp && user->morale >= tech2_cost) {
    user->morale -= tech2_cost;
    PLOGI << "Decreased Morale to: " << user->morale;
    return true;
  }
  else if (user->life >= tech2_cost) {
    user->increaseExhaustion(tech2_cost);
    return true;
  }
  else {
    PLOGI << "Player does not meet conditons to use Heavy Tech.";
    return false;
  }
}

std::unique_ptr<CombatAction> SubWeapon::lightTechnique() {
  PLOGI << "Light Technique is not available.";
  return nullptr;
}

std::unique_ptr<CombatAction> SubWeapon::heavyTechnique() {
  PLOGI << "Heavy Technique is not available.";
  return nullptr;
}
