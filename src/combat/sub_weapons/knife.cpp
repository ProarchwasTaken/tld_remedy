#include <memory>
#include "enums.h"
#include "base/sub_weapon.h"
#include "base/combat_action.h"
#include "system/sound_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/knife_cleave.h"
#include "combat/actions/knife_piercer.h"
#include "combat/sub_weapons/knife.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;
SoundAtlas Knife::sfx("knife");


Knife::Knife(Mary *user) : SubWeapon("Knife", SubWeaponID::KNIFE, user) {
  *tech1 = {"Cleave", TechCostType::MORALE, 8};
  *tech2 = {"Piercer", TechCostType::MORALE, 12};

  sfx.use();
  PLOGI << "Knife Sub-Weapon: Initialized.";
}

Knife::~Knife() {
  sfx.release();
  PLOGI << "Knife SubWeapon: Cleared from memory.";
}

unique_ptr<CombatAction> Knife::lightTechnique() {
  user->morale -= tech1->cost;
  PLOGI << "Decreased Morale to: " << user->morale;
  return make_unique<KnifeCleave>(user);
}

unique_ptr<CombatAction> Knife::heavyTechnique() {
  user->morale -= tech2->cost;
  PLOGI << "Decreased Morale to: " << user->morale;
  return make_unique<KnifePiercer>(user);
}
