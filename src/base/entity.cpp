#include <cassert>
#include <raylib.h>
#include <vector>
#include <memory>
#include <plog/Log.h>
#include "data/rect_ex.h"
#include "base/entity.h"

using std::vector, std::unique_ptr;


void Entity::clear(vector<unique_ptr<Entity>> &entity_list) {
  PLOGI << "Clearing all entities in the current scene from memory.";
  for (unique_ptr<Entity> &entity : entity_list) {
    entity.reset();
  }

  entity_list.clear();
}


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
  assert(existing_entities.find(entity_id) == existing_entities.end());

  int iteration = 0;
  bool assigned_id = false;

  while (!assigned_id) {
    assigned_id = existing_entities.emplace(iteration).second;

    if (!assigned_id) {
      iteration++;
    }
    else {
      int &new_id = const_cast<int&>(entity_id);
      new_id = iteration;
    }
  }
}

void Entity::rectExCorrection(RectEx &rect_ex) {
  rect_ex.position.x = position.x + rect_ex.offset.x;
  rect_ex.position.y = position.y + rect_ex.offset.y;

  rect_ex.rect = {
    rect_ex.position.x, 
    rect_ex.position.y,
    rect_ex.scale.x,
    rect_ex.scale.y
  };
}

void Entity::drawDebug() {
  DrawRectangleLinesEx(bounding_box.rect, 1, BLUE);
  DrawCircleV(position, 1, BLUE);
}
