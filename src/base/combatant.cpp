#include <cassert>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "base/combatant.h"

using std::string;


Combatant::Combatant(string name, CombatantType combatant_type,
                     Vector2 position, Direction direction)
{
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::COMBATANT;
  this->combatant_type = combatant_type;
  bool successful = existing_combatants.emplace(this).second;

  assert(successful);
  PLOGI << "COMBATANT: '" << name << "'[ID: " << entity_id << "]";
}

Combatant::~Combatant() {
  int erased = existing_combatants.erase(this);
  assert(erased == 1);
  PLOGI << "Removed combatant: '" << name << "'";
}

void Combatant::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hurtbox.rect, 1, RED);
}
