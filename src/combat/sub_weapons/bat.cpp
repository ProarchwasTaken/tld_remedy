#include <memory>
#include "enums.h"
#include "base/sub_weapon.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/bat_swing.h"
#include "combat/sub_weapons/bat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


Bat::Bat(Mary *user) : SubWeapon("Bat", SubWeaponID::BAT, user) {
  *tech1 = {"Swing", TechCostType::MORALE, 3.6};
  *tech2 = {"Hail Mary", TechCostType::MORALE, 3.6};

  PLOGI << "Bat Sub-Weapon: Initialized.";
}

Bat::~Bat() {
  PLOGI << "Bat Sub-Weapon: Cleared from memory.";
}

unique_ptr<CombatAction> Bat::lightTechnique() {
  float cost = user->calculateMoraleCost(tech1->cost);
  user->morale -= cost;
  PLOGI << "Decreased Morale to: " << user->morale;
  return make_unique<BatSwing>(user);
}
