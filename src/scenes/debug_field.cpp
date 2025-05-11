#include <cassert>
#include <memory>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/actor.h"
#include "data/actor.h"
#include "utils/camera.h"
#include "actors/player.h"
#include "scenes/debug_field.h"

using std::unique_ptr, std::make_unique;

DebugField::DebugField() {
  field.loadMap("db_01");
  setupActors();

  camera = CameraUtils::setupField();
  camera_target = Actor::getActor(ActorType::PLAYER);
  camera.target = camera_target->position;
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

void DebugField::setupActors() {
  PLOGI << "Setting up field actors...";
  for (ActorData data : field.actor_queue) {
    Vector2 position = data.position;
    Direction direction = data.direction;

    unique_ptr<Entity> entity;

    switch (data.type) {
      case ActorType::PLAYER: {
        entity = make_unique<PlayerActor>(position, direction);
        break;
      }
      default: {

      }
    }

    if (entity != nullptr) {
      entities.push_back(std::move(entity));
    }
  }

  field.actor_queue.clear();
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

  CameraUtils::followFieldEntity(camera, camera_target);
}

void DebugField::draw() {
  if (!field.ready) {
    return;
  }

  BeginMode2D(camera); 
  {
    field.draw();

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
      entity->drawDebug();
    }

    if (Game::debugInfo()) {
      field.drawCollLines();
    }
  }
  EndMode2D();
}
 
