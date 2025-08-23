#include "enums.h"
#include "base/sub_weapon.h"
#include "combat/combatants/party/mary.h"
#include "combat/sub_weapons/knife.h"

Knife::Knife(Mary *user) : SubWeapon("Knife", SubWeaponID::KNIFE, user) {
  tech1_cost = 8;
  tech1_type = TechCostType::MORALE;

  tech2_cost = 12;
  tech2_type = TechCostType::MORALE;
}
