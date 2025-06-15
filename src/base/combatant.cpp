#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "base/combatant.h"

using std::string;
int Combatant::enemy_count = 0;


Combatant *Combatant::getCombatantByID(int entity_id) {
  Combatant *result = NULL;

  for (Combatant *combatant : existing_combatants) {
    if (combatant->entity_id == entity_id) {
      result = combatant;
      break;
    }
  }

  return result;
}

Combatant::Combatant(string name, CombatantType combatant_type,
                     Vector2 position, Direction direction)
{
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::COMBATANT;
  this->combatant_type = combatant_type;
  if (combatant_type == CombatantType::ENEMY) {
    enemy_count++;
  }
 
  bool successful = existing_combatants.emplace(this).second;
  assert(successful);

  state = CombatantState::NEUTRAL;
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]";
}

Combatant::~Combatant() {
  int erased = existing_combatants.erase(this);
  assert(erased == 1);

  if (combatant_type == CombatantType::ENEMY) {
    enemy_count--;
  }
  PLOGI << "Removed combatant: '" << name << "'";
}

void Combatant::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hurtbox.rect, 1, RED);
}
