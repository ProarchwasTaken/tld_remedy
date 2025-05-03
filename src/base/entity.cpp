#include <raylib.h>
#include <plog/Log.h>
#include "base/entity.h"


Entity::Entity() {
  assignID();
  PLOGI << "Entity [ID: " << entity_id << "] has been created.";
}

void Entity::assignID() {
  int iteration = 0;
  bool assigned_id = false;

  while (!assigned_id) {
    assigned_id = existing_entities.emplace(iteration).second;

    if (!assigned_id) {
      iteration++;
    }
    else {
      entity_id = iteration;
    }
  }
}

void Entity::boundRectCorrection() {
  bound.x = position.x - bound_offset.x;
  bound.y = position.y - bound_offset.y;
}

void Entity::drawDebug() {
  DrawRectangleLinesEx(bound, 2, BLUE);
}
