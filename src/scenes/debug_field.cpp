#include <cassert>
#include <memory>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "base/entity.h"
#include "base/actor.h"
#include "actors/player.h"
#include "scenes/debug_field.h"

using std::unique_ptr, std::make_unique;

DebugField::DebugField() {
  field.loadMap("db_01");
  entities.push_back(
    make_unique<PlayerActor>((Vector2){213, 120}, Direction::DOWN)
  );
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
  if (!field.ready) {
    return;
  }

  for (Actor *actor : Actor::existing_actors) {
    actor->behavior();
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }
}

void DebugField::draw() {
  if (!field.ready) {
    return;
  }

  field.draw();

  for (unique_ptr<Entity> &entity : entities) {
    entity->draw();
    entity->drawDebug();
  }
}
 
