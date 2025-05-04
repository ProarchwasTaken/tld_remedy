#include <cassert>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "base/actor.h"

using std::string;


Actor::Actor(string name, Vector2 position, enum Direction direction) {
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::ACTOR;
  bool successful = existing_actors.emplace(this).second;

  assert(successful);
  PLOGI << "ACTOR: '" << name << "' [ID: " << entity_id << "]";
}

Actor::~Actor() {
  int erased = existing_actors.erase(this);
  assert(erased == 1);
  PLOGI << "Removed actor: '" << name << "'";
}

void Actor::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(collis_box.rect, 1, RED);
}
