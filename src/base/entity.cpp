#include <cassert>
#include <raylib.h>
#include <plog/Log.h>
#include "data/rect_ex.h"
#include "base/entity.h"


Entity::Entity() {
  assignID();
  PLOGI << "Entity [ID: " << entity_id << "] has been created.";
}

Entity::~Entity() {
  PLOGI << "Clearing Entity [ID: " << entity_id << "] from memory.";


  int erased = existing_entities.erase(entity_id);
  assert(erased == 1);
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

void Entity::rectExCorrection(RectEx &rect_ex) {
  rect_ex.rect.x = position.x - rect_ex.offset.x;
  rect_ex.rect.y = position.y - rect_ex.offset.y;
}

void Entity::drawDebug() {
  DrawRectangleLinesEx(bounding_box.rect, 2, BLUE);
}
