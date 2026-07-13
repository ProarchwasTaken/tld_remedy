#include "enums.h"
#include "base/sub_weapon.h"
#include "combat/combatants/party/mary.h"
#include "combat/sub_weapons/bat.h"
#include <plog/Log.h>


Bat::Bat(Mary *user) : SubWeapon("Bat", SubWeaponID::BAT, user) {
  *tech1 = {"Swing", TechCostType::MORALE, 3.6};
  *tech2 = {"Hail Mary", TechCostType::MORALE, 3.6};

  PLOGI << "Bat Sub-Weapon: Initialized.";
}

Bat::~Bat() {
  PLOGI << "Bat Sub-Weapon: Cleared from memory.";
}
