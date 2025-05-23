#include <cassert>
#include <memory>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/actor.h"
#include "data/actor.h"
#include "data/field_event.h"
#include "system/field_handler.h"
#include "utils/camera.h"
#include "actors/player.h"
#include "entities/map_trans.h"
#include "scenes/field.h"

#ifndef NDEBUG
#include "system/field_commands.h"
#endif // !NDEBUG

using std::unique_ptr, std::make_unique, std::string;


FieldScene::FieldScene() {
  camera = CameraUtils::setupField();
  mapLoadProcedure("db_01");

  #ifndef NDEBUG
  static CommandSystem command_system(this);
  #endif // !NDEBUG
  PLOGI << "Initialized the Field Scene.";
}

FieldScene::~FieldScene() {
  Entity::clear(entities);

  assert(Actor::existing_actors.empty());
  assert(Entity::existing_entities.empty());
  PLOGI << "Unloaded the Field scene.";
}

void FieldScene::mapLoadProcedure(string map_name, string *spawn_name) {
  PLOGI << "Running map load procedure";
  float start_time = GetTime();

  if (!entities.empty()) {
    Entity::clear(entities);
  }

  field.loadMap(map_name, spawn_name);
  setupActors();
  setupMapTransitions();

  camera_target = Actor::getActor(ActorType::PLAYER);
  camera.target = camera_target->position;

  PLOGI << "Procedure complete.";
  float elapsed_time = GetTime() - start_time;
  PLOGI << "Loading Time: " << elapsed_time;

  map_ready = true;
}

void FieldScene::setupActors() {
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


void FieldScene::setupMapTransitions() {
  PLOGI << "Setting up map transition triggers...";
  for (MapTransData data : field.map_trans_queue) {
    unique_ptr<Entity> entity;

    entity = make_unique<MapTransition>(data);
    entities.push_back(std::move(entity));
  }

  field.map_trans_queue.clear();
}

void FieldScene::update() { 
  if (!map_ready) {
    mapLoadProcedure(next_map.map_name, &next_map.spawn_point);
    Game::fadein(0.10);
    return;
  }

  #ifndef NDEBUG
  CommandSystem::process();
  #endif // !NDEBUG

  for (Actor *actor : Actor::existing_actors) {
    actor->behavior();
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  CameraUtils::followFieldEntity(camera, camera_target);

  EventPool<FieldEvent> *event_pool = field_handler.get();
  if (!event_pool->empty()) {
    PLOGI << "Field Events raised: " << event_pool->size();
    for (auto &event : *event_pool) {
      fieldEventHandling(event);
    }

    FieldEventHandler::clear();
  }
}

void FieldScene::fieldEventHandling(std::unique_ptr<FieldEvent> &event) {
  switch (event->event_type) {
    case FieldEventType::LOAD_MAP: {
      PLOGD << "Event detected: LoadMapEvent";
      auto *event_data = static_cast<LoadMapEvent*>(event.get());

      string map_name = event_data->map_name;
      string *spawn_name = &event_data->spawn_point;

      PLOGI << "Preparing to load map: '" << map_name << "' at " <<
        "spawnpoint: '" << *spawn_name << "'";
      next_map = *event_data;
      Game::fadeout(0.10);
      map_ready = false;
      break;
    }
  }
}

void FieldScene::draw() {
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

  #ifndef NDEBUG
  CommandSystem::drawBuffer();
  #endif // !NDEBUG
}
 
