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

  switch (tech1->type) {
    case TechCostType::LIFE: {
      if (user->life > 1) {
        return true;
      }
      else {
        PLOGI << "Player does not have enough Life!";
        return false;
      }
    }
    case TechCostType::MORALE: {
      float cost = user->calculateMoraleCost(tech1->cost);
      if (user->morale >= cost) {
        return true;
      }
      else {
        PLOGI << "Player does not have enough Morale!";
        return false;
      }
    }
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

  switch (tech2->type) {
    case TechCostType::LIFE: {
      if (user->life > 1) {
        return true;
      }
      else {
        PLOGI << "Player does not have enough Life!";
        return false;
      }
    }
    case TechCostType::MORALE: {
      float cost = user->calculateMoraleCost(tech2->cost);
      if (user->morale >= cost) {
        return true;
      }
      else {
        PLOGI << "Player does not have enough Morale!";
        return false;
      }
    }
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
