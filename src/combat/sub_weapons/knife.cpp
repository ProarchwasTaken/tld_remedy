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
  *tech1 = {"Cleave", TechCostType::MORALE, 3.4};
  *tech2 = {"Piercer", TechCostType::MORALE, 7.4};

  sfx.use();
  PLOGI << "Knife Sub-Weapon: Initialized.";
}

Knife::~Knife() {
  sfx.release();
  PLOGI << "Knife SubWeapon: Cleared from memory.";
}

unique_ptr<CombatAction> Knife::lightTechnique() {
  float cost = user->calculateMoraleCost(tech1->cost);
  user->morale -= cost;
  PLOGI << "Decreased Morale to: " << user->morale;
  return make_unique<KnifeCleave>(user);
}

unique_ptr<CombatAction> Knife::heavyTechnique() {
  float cost = user->calculateMoraleCost(tech2->cost);
  user->morale -= cost;
  PLOGI << "Decreased Morale to: " << user->morale;
  return make_unique<KnifePiercer>(user);
}
