#include <cstddef>
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
  tech1 = &user->tech1;
  tech2 = &user->tech2;

  PLOGI << "Sub-Weapon: '" << name << "' has been assigned to the"
    " Player Combatant.";
}

bool SubWeapon::lightTechCondition() {
  if (tech1 == NULL) {
    PLOGD << "tech1 pointer is NULL";
    return false;
  }

  if (user->demoralized) {
    PLOGI << "Player is Despondent. Unable to use Technique!";
    return false;
  }

  bool using_mp = tech1->type == TechCostType::MORALE;
  if (using_mp && user->morale >= tech1->cost) {
    return true;
  }
  else if (!using_mp && user->life >= tech1->cost) {
    return true;
  }
  else {
    PLOGI << "Player does not meet conditions to use Light Tech.";
    return false;
  }
}

bool SubWeapon::heavyTechCondition() {
  if (tech2 == NULL) {
    PLOGD << "tech2 pointer is NULL.";
    return false;
  }

  if (user->demoralized) {
    PLOGI << "Player is Despondent. Unable to use Technique!";
    return false;
  }

  bool using_mp = tech2->type == TechCostType::MORALE;
  if (using_mp && user->morale >= tech2->cost) {
    return true;
  }
  else if (!using_mp && user->life >= tech2->cost) {
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
