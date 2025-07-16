#include <algorithm>
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
#include "system/sound_atlas.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "utils/text.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"
#include "field/actors/enemy.h"
#include "field/entities/map_trans.h"
#include "field/entities/pickup.h"
#include "scenes/field.h"
#ifndef NDEBUG
#include "field/system/field_commands.h"
#endif // !NDEBUG

using std::unique_ptr, std::make_unique, std::string, std::vector;
bool fieldAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2);

SoundAtlas FieldScene::sfx("field");


FieldScene::FieldScene(SubWeaponID sub_weapon, CompanionID companion) {
  PLOGI << "Starting a new session.";
  session.version = Game::session_version;

  session.player.sub_weapon = sub_weapon;
  initCompanionData(companion);

  setup();
}

FieldScene::FieldScene(Session *session_data) {
  assert(session_data != NULL);

  PLOGI << "Loading existing session data.";
  session = *session_data;
  setup();
}

FieldScene::~FieldScene() {
  Entity::clear(entities);
  sfx.release();

  assert(Actor::existing_actors.empty());
  assert(Entity::existing_entities.empty());
  PLOGI << "Unloaded the Field scene.";
}

void FieldScene::setup() {
  PLOGI << "Setting up the field scene...";
  scene_id = SceneID::FIELD;

  mapLoadProcedure(session.location);

  #ifndef NDEBUG
  static CommandSystem command_system;
  command_system.assignScene(this);
  #endif // !NDEBUG
  
  sfx.use();
  Game::fadein(1.0);
  PLOGI << "Field scene is ready to go!";
}

void FieldScene::initCompanionData(CompanionID id) {
  PLOGI << "Initializing Companion data...";
  Companion *companion = &session.companion;

  switch (id) {
    case CompanionID::ERWIN: {
      std::strcpy(companion->name, "Erwin");
      companion->id = id;

      companion->life = 20;
      companion->max_life = 20;

      companion->init_morale = 8;
      companion->max_morale = 25;

      companion->offense = 10;
      companion->defense = 5;
      companion->intimid = 8;
      companion->persist = 6;
    }
  }

  PLOGI << "Initialized data for: " << companion->name;
}

void FieldScene::mapLoadProcedure(string map_name, string *spawn_name) {
  PLOGI << "Running map load procedure";
  float start_time = GetTime();
  ActorHandler::clearEvents();

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
      CompanionID id = session.companion.id;
      entity = make_unique<CompanionActor>(id, position, direction);
      break;
    }
    case ActorType::ENEMY: {
      EnemyActorData *enemy_data = static_cast<EnemyActorData*>(data);
      vector<Direction> routine = enemy_data->routine;
      float speed = enemy_data->speed;

      entity = make_unique<EnemyActor>(position, routine, speed);
      break;
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
    Game::fadein(0.25);
    return;
  }

  #ifndef NDEBUG
  CommandSystem::process();
  #endif // !NDEBUG
  
  for (Actor *actor : Actor::existing_actors) {
    actor->behavior();
  }
  actor_handler.clearEvents();

  if (Game::state() == GameState::SLEEP) {
    return;
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  camera.follow(camera_target);
  eventProcessing();
}

void FieldScene::eventProcessing() {
  EventPool<FieldEvent> *event_pool = evt_handler.get();
  if (!event_pool->empty()) {
    PLOGI << "Field Events raised: " << event_pool->size();
    for (auto &event : *event_pool) {
      eventHandling(event);
    }

    evt_handler.clear();
  }

  actor_handler.transferEvents();
}

void FieldScene::eventHandling(unique_ptr<FieldEvent> &event) {
  switch (event->event_type) {
    case FieldEVT::LOAD_MAP: {
      PLOGD << "Event detected: LoadMapEvent";
      auto *event_data = static_cast<LoadMapEvent*>(event.get());

      string map_name = event_data->map_name;
      string *spawn_name = &event_data->spawn_point;

      PLOGI << "Preparing to load map: '" << map_name << "' at " <<
        "spawnpoint: '" << *spawn_name << "'";
      next_map = *event_data;
      Game::fadeout(0.25);
      map_ready = false;
      break;
    }
    case FieldEVT::SAVE_SESSION: {
      PLOGD << "Event Detected: SaveSessionEvent";

      PLOGI << "Saving the session.";
      Game::saveSession(&session);
      break;
    }
    case FieldEVT::LOAD_SESSION: {
      PLOGD << "Event Detected: LoadSessionEvent";

      PLOGI << "Loading session.";
      Game::loadSession();
      break;
    }
    case FieldEVT::INIT_COMBAT: {
      PLOGI << "Event Detected: InitCombatEvent";

      Game::initCombat(&session);
      break;
    }
    case FieldEVT::DELETE_ENTITY: {
      PLOGD << "Event detected: DeleteEntityEvent";
      auto *event_data = static_cast<DeleteEntityEvent*>(event.get());

      int entity_id = event_data->entity_id;

      PLOGI << "Attempting to delete Entity [ID: " << entity_id << "]";
      deleteEntity(entity_id);
      break;
    }
    case FieldEVT::UPDATE_COMMON_DATA: {
      PLOGD << "Event detected: UpdateCommonEvent";
      auto *event_data = static_cast<UpdateCommonEvent*>(event.get());

      int object_id = event_data->object_id;
      bool active = event_data->active;

      PLOGI << "Updating common data associated with Object ID: " 
        << object_id;
      updateCommonData(object_id, active);
      break;
    }
    case FieldEVT::CHANGE_SUPPLIES: {
      PLOGD << "Event detected: SetSuppliesEvent";
      auto *event_data = static_cast<SetSuppliesEvent*>(event.get());

      int value = event_data->value;

      PLOGI << "Changing value of 'supplies' to: " << value;
      session.supplies = value;
      break;
    }
    case FieldEVT::ADD_SUPPLIES: {
      PLOGD << "Event detected: AddSuppliesEvent";
      auto *event_data = static_cast<AddSuppliesEvent*>(event.get());

      int magnitude = event_data->magnitude;

      PLOGI << "Incrementing value of 'supplies' by: " << magnitude;
      session.supplies += magnitude;
      break;
    }
    case FieldEVT::CHANGE_PLR_LIFE: {
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
  std::sort(entities.begin(), entities.end(), fieldAlgorithm);

  BeginMode2D(camera); 
  {
    field.draw();

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
    }

    #ifndef NDEBUG
    if (Game::debugInfo()) {
      for (unique_ptr<Entity> &entity : entities) {
        entity->drawDebug();
      }

      field.drawCollLines();
    } 
    #endif // !NDEBUG
  }
  EndMode2D();

  #ifndef NDEBUG
  CommandSystem::drawBuffer();
  if (Game::debugInfo()) drawSessionInfo();
  #endif // !NDEBUG
}

bool fieldAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2) {
  if (e1->entity_type != e2->entity_type) {
    return e1->entity_type > e2->entity_type;
  }
  else {
    return e1->position.y < e2->position.y;
  }
}

void FieldScene::drawSessionInfo() {
  Font *font = &Game::sm_font;
  int text_size = Game::sm_font.baseSize;
  float base_x = 420;
  float y = 4;
  float spacing = 9;

  string location = TextFormat("Location: %s", session.location);
  Vector2 loc_pos = TextUtils::alignRight(location.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  string supplies = TextFormat("Supplies: %03i", session.supplies);
  Vector2 sup_pos = TextUtils::alignRight(supplies.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  Player *player = &session.player;
  string plr_hp = TextFormat("%s Life: %02.00f / %02.00f", player->name, 
                             player->life, player->max_life);
  Vector2 php_pos = TextUtils::alignRight(plr_hp.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;



  Companion *companion = &session.companion;
  string com_hp = TextFormat("%s Life: %02.00f / %02.00f", 
                             companion->name, companion->life,
                             companion->max_life);
  Vector2 chp_pos = TextUtils::alignRight(com_hp.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  string pursue = TextFormat("Persuing Enemy: %i", 
                             EnemyActor::pursuing_enemy);
  Vector2 per_pos = TextUtils::alignRight(pursue.c_str(), {base_x, y}, 
                                          *font, -3, 0);


  DrawTextEx(*font, location.c_str(), loc_pos, text_size, -3, GREEN);
  DrawTextEx(*font, supplies.c_str(), sup_pos, text_size, -3, GREEN);
  DrawTextEx(*font, plr_hp.c_str(), php_pos, text_size, -3, GREEN);
  DrawTextEx(*font, com_hp.c_str(), chp_pos, text_size, -3, GREEN);
  DrawTextEx(*font, pursue.c_str(), per_pos, text_size, -3, GREEN);
}
