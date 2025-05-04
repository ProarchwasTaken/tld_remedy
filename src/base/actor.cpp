#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"

using std::string;


Actor::Actor(string name, Vector2 position, enum Direction direction) {
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::ACTOR;
}

void Actor::drawDebug() {
  DrawRectangleLinesEx(collis_box.rect, 2, RED);
}
