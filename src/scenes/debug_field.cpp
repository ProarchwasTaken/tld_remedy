#include <cassert>
#include <memory>
#include <raylib.h>
#include <plog/Log.h>
#include "base/entity.h"
#include "base/actor.h"
#include "scenes/debug_field.h"

using std::unique_ptr, std::make_unique;

DebugField::DebugField() {
  PLOGI << "Initialized the DebugField Scene.";
}

DebugField::~DebugField() {
  for (unique_ptr<Entity> &entity : entities) {
    entity.reset();
  }
  entities.clear();

  assert(Actor::existing_actors.empty());
  assert(Entity::existing_entities.empty());
  PLOGI << "Unloaded the DebugField scene.";
}

void DebugField::update() {
  for (Actor *actor : Actor::existing_actors) {
    actor->behavior();
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }
}

void DebugField::draw() {
  for (unique_ptr<Entity> &entity : entities) {
    entity->draw();
  }
}
 
