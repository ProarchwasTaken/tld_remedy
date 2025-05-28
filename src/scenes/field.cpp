#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/actor.h"
#include "data/actor.h"
#include "data/entity.h"
#include "data/field_event.h"
#include "data/session.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "utils/camera.h"
#include "utils/text.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"
#include "field/entities/map_trans.h"
#include "field/entities/pickup.h"
#include "scenes/field.h"

#ifndef NDEBUG
#include "field/system/field_commands.h"
#endif // !NDEBUG

using std::unique_ptr, std::make_unique, std::string, std::vector;


FieldScene::FieldScene(Session *session_data) {
  if (session_data != NULL) {
    PLOGI << "Loading existing session data.";
    session = *session_data;
  }

  camera = CameraUtils::setupField();
  mapLoadProcedure(session.location);

  #ifndef NDEBUG
  static CommandSystem command_system;
  command_system.assignScene(this);
  #endif // !NDEBUG
  
  Game::fadein(1.0);
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

  field.loadMap(session, map_name, spawn_name);
  setupEntities();

  camera_target = Actor::getActor(ActorType::PLAYER);
  camera.target = camera_target->position;

  std::strcpy(session.location, map_name.c_str());
  map_ready = true;

  PLOGI << "Procedure complete.";
  float elapsed_time = GetTime() - start_time;
  PLOGI << "Load Time: " << elapsed_time;
}

void FieldScene::setupActor(ActorData *data) {
  unique_ptr<Entity> entity;
  Vector2 position = data->position;
  Direction direction = data->direction;

  switch (data->actor_type) {
    case ActorType::PLAYER: {
      entity = make_unique<PlayerActor>(position, direction);
      break;
    }
    case ActorType::COMPANION: {
      entity = make_unique<CompanionActor>(position, direction);
      break;
    }
    default: {

    }
  }

  if (entity != nullptr) {
    entities.push_back(std::move(entity));
  }
}


void FieldScene::setupEntities() {
  PLOGI << "Setting up field entities...";
  PLOGD << "Entities to be created: " << field.entity_queue.size();

  for (auto &data : field.entity_queue) {
    unique_ptr<Entity> entity;

    switch (data->type) {
      case EntityType::ACTOR: {
        setupActor(static_cast<ActorData*>(data.get()));
        break;
      }
      case EntityType::MAP_TRANSITION: {
        MapTransData *trans_data = static_cast<MapTransData*>(data.get());
        entity = make_unique<MapTransition>(*trans_data);
        break;
      }
      case EntityType::PICKUP: {
        PickupData *pick_data = static_cast<PickupData*>(data.get());
        entity = make_unique<Pickup>(*pick_data);
        break;
      }
      default: {

      }
    }

    if (entity != nullptr) {
      entities.push_back(std::move(entity));
    }

    data.reset();
  }

  field.entity_queue.clear();
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
    ActorHandler::clearEvents();
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  CameraUtils::followFieldEntity(camera, camera_target);
  eventProcessing();
}

void FieldScene::eventProcessing() {
  EventPool<FieldEvent> *event_pool = field_handler.get();
  if (!event_pool->empty()) {
    PLOGI << "Field Events raised: " << event_pool->size();
    for (auto &event : *event_pool) {
      fieldEventHandling(event);
    }

    FieldHandler::clear();
  }

  ActorHandler::transferEvents();
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
    case FieldEventType::SAVE_SESSION: {
      PLOGD << "Event Detected: SaveSessionEvent";

      PLOGI << "Saving the session.";
      Game::saveSession(&session);
      break;
    }
    case FieldEventType::LOAD_SESSION: {
      PLOGD << "Event Detected: LoadSessionEvent";

      PLOGI << "Loading session.";
      Game::loadSession();
      break;
    }
    case FieldEventType::DELETE_ENTITY: {
      PLOGD << "Event detected: DeleteEntityEvent";
      auto *event_data = static_cast<DeleteEntityEvent*>(event.get());

      int entity_id = event_data->entity_id;

      PLOGI << "Attempting to delete Entity [ID: " << entity_id << "]";
      deleteEntity(entity_id);
      break;
    }
    case FieldEventType::UPDATE_COMMON_DATA: {
      PLOGD << "Event detected: UpdateCommonEvent";
      auto *event_data = static_cast<UpdateCommonEvent*>(event.get());

      int object_id = event_data->object_id;
      bool active = event_data->active;

      PLOGI << "Updating common data associated with Object ID: " 
        << object_id;
      updateCommonData(object_id, active);
      break;
    }
    case FieldEventType::CHANGE_SUPPLIES: {
      PLOGD << "Event detected: SetSuppliesEvent";
      auto *event_data = static_cast<SetSuppliesEvent*>(event.get());

      int value = event_data->value;

      PLOGI << "Changing value of 'supplies' to: " << value;
      session.supplies = value;
      break;
    }
    case FieldEventType::ADD_SUPPLIES: {
      PLOGD << "Event detected: AddSuppliesEvent";
      auto *event_data = static_cast<AddSuppliesEvent*>(event.get());

      int magnitude = event_data->magnitude;

      PLOGI << "Incrementing value of 'supplies' by: " << magnitude;
      session.supplies += magnitude;
      break;
    }
    case FieldEventType::CHANGE_PLR_LIFE: {
      PLOGD << "Event detected: SetPlrLifeEvent";
      auto *event_data = static_cast<SetPlrLifeEvent*>(event.get());

      float value = event_data->value;

      PLOGI << "Changing the player's life expectancy to: " << value;
      session.player.life = value;
      break;
    }
  }
}

void FieldScene::updateCommonData(int object_id, bool active) {
  int common_count = session.common_count;
  for (int x = 0; x < common_count; x++) {
    CommonData *data = &session.common[x];

    string map_name = data->map_name;

    if (map_name != session.location) {
      continue;
    }

    if (object_id == data->object_id) {
      data->active = active;
      return;
    }
  }

  PLOGE << "Failed to find common data!";
}

void FieldScene::deleteEntity(int entity_id) {
  vector<unique_ptr<Entity>> temporary;

  for (auto &entity : entities) {
    if (entity->entity_id == entity_id) {
      PLOGD << "Found Entity [ID: " << entity_id << "]";
      entity.reset();
    }
    else {
      unique_ptr<Entity> temp_entity = nullptr;
      entity.swap(temp_entity);

      temporary.push_back(std::move(temp_entity));
    }
  }

  entities.clear();
  temporary.swap(entities);
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
  if (Game::debugInfo()) drawSessionInfo();
  #endif // !NDEBUG
}

void FieldScene::drawSessionInfo() {
  int text_size = Game::sm_font.baseSize;
  float base_x = 420;
  float y = 4;

  string base = "Location: ";
  string extension = session.location;
  string location = base + extension;
  Vector2 loc_pos = TextUtils::alignRight(location.c_str(), {base_x, y}, 
                                          Game::sm_font, -3, 0);
  y += 8;

  string supplies = TextFormat("Supplies: %03i", session.supplies);
  Vector2 sup_pos = TextUtils::alignRight(supplies.c_str(), {base_x, y}, 
                                          Game::sm_font, -3, 0);
  y += 8;

  string plr_hp = TextFormat("Player Life: %02.00f / %02.00f",
                             session.player.life, 
                             session.player.max_life);
  Vector2 php_pos = TextUtils::alignRight(plr_hp.c_str(), {base_x, y}, 
                                          Game::sm_font, -3, 0);
  y += 8;

  string plr_mp = TextFormat("Player Morale: %02.00f / %02.00f",  
                             session.player.init_morale, 
                             session.player.max_morale);
  Vector2 pmp_pos = TextUtils::alignRight(plr_mp.c_str(), {base_x, y}, 
                                          Game::sm_font, -3, 0);

  DrawTextEx(Game::sm_font, location.c_str(), loc_pos, text_size, -3, 
             GREEN);
  DrawTextEx(Game::sm_font, supplies.c_str(), sup_pos, text_size, -3, 
             GREEN);
  DrawTextEx(Game::sm_font, plr_hp.c_str(), php_pos, text_size, -3, 
             GREEN);
  DrawTextEx(Game::sm_font, plr_mp.c_str(), pmp_pos, text_size, -3, 
             GREEN);
}
 
