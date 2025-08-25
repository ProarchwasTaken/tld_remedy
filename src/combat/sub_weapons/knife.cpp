#include <memory>
#include "enums.h"
#include "base/sub_weapon.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/knife_cleave.h"
#include "combat/sub_weapons/knife.h"

using std::unique_ptr, std::make_unique;


Knife::Knife(Mary *user) : SubWeapon("Knife", SubWeaponID::KNIFE, user) {
  tech1_cost = 8;
  tech1_type = TechCostType::MORALE;

  tech2_cost = 12;
  tech2_type = TechCostType::MORALE;
}

unique_ptr<CombatAction> Knife::lightTechnique() {
  return make_unique<KnifeCleave>(user);
}
