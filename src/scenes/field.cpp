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
#include "data/actor_event.h"
#include "data/session.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "menu/panels/dialog.h"
#include "field/system/field_map.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"
#include "field/actors/enemy.h"
#include "field/entities/map_trans.h"
#include "field/entities/pickup.h"
#include "field/entities/save_point.h"
#include "field/sequences/save.h"
#include "field/sequences/rest.h"
#include "scenes/field.h"
#ifndef NDEBUG
#include "field/system/field_commands.h"
#include "field/sequences/db_01.h"
#endif // !NDEBUG

using std::unique_ptr, std::make_unique, std::string, std::vector;
bool fieldAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2);

SoundAtlas FieldScene::sfx("field");
SpriteAtlas FieldScene::emotes("entities", "emote_balloons");


FieldScene::FieldScene(SubWeaponID sub_weapon, CompanionID companion) {
  PLOGI << "Starting a new session.";
  session = make_unique<Session>();
  session->version = Game::session_version;

  initPlayerData(sub_weapon);
  initCompanionData(companion);

  setup();
}

FieldScene::FieldScene(Session *session_data) {
  assert(session_data != NULL);

  PLOGI << "Loading existing session data.";
  session = make_unique<Session>();
  *session = *session_data;
  setup();
}

FieldScene::~FieldScene() {
  if (sequence != nullptr) {
    sequence.reset();
  }

  Entity::clear(entities);
  assert(Actor::existing_actors.empty());

  UnloadTexture(vignette);
  field.reset();
  session.reset();

  sfx.release();

  if (panel != nullptr) {
    panel.reset();
  }
  PLOGI << "Unloaded the Field scene.";
}

void FieldScene::setup() {
  PLOGI << "Setting up the field scene...";
  scene_id = SceneID::FIELD;

  field = make_unique<FieldMap>();
  mapLoadProcedure(session->map_name);

  #ifndef NDEBUG
  static CommandSystem command_system;
  command_system.assignScene(this);
  #endif // !NDEBUG
  
  sfx.use();
  vignette = LoadTexture("graphics/overlays/field_vignette.png");
  PLOGI << "Field scene is ready to go!";
}

void FieldScene::initPlayerData(SubWeaponID weapon_id) {
  PLOGI << "initializing Player data...";
  Player *player = &session->player;

  std::strcpy(player->name, "Mary");
  player->member_id = PartyMemberID::MARY;
  player->weapon_id = weapon_id;

  player->life = 15;
  player->max_life = 15;

  player->init_morale = 10;
  player->max_morale = 25;

  player->offense = 6;
  player->defense = 4;
  player->intimid = 6;
  player->persist = 4;

  switch (weapon_id) {
    case SubWeaponID::KNIFE: {
      PLOGI << "Applying Knife status bonuses.";
      player->offense += 2;
      player->intimid += 1;
      break;
    }
  }
}

void FieldScene::initCompanionData(CompanionID companion_id) {
  PLOGI << "Initializing Companion data...";
  Companion *companion = &session->companion;

  switch (companion_id) {
    case CompanionID::ERWIN: {
      std::strcpy(companion->name, "Erwin");
      companion->member_id = PartyMemberID::ERWIN;
      companion->companion_id = companion_id;

      companion->life = 20;
      companion->max_life = 20;

      companion->init_morale = 5;
      companion->max_morale = 20;

      companion->offense = 8;
      companion->defense = 6;
      companion->intimid = 7;
      companion->persist = 5;
      break;
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

  field->loadMap(*session, map_name, spawn_name);
  setupEntities();

  camera_target = Actor::getActor(ActorType::PLAYER);
  camera.target = camera_target->position;

  std::strcpy(session->map_name, map_name.c_str());
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
      CompanionID id = session->companion.companion_id;
      entity = make_unique<CompanionActor>(id, position, direction);
      break;
    }
    case ActorType::ENEMY: {
      EnemyActorData *enemy_data = static_cast<EnemyActorData*>(data);
      entity = make_unique<EnemyActor>(*enemy_data);
      break;
    }
  }

  if (entity != nullptr) {
    entities.push_back(std::move(entity));
  }
}


void FieldScene::setupEntities() {
  PLOGI << "Setting up field entities...";
  PLOGD << "Entities to be created: " << field->entity_queue.size();

  for (auto &data : field->entity_queue) {
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
      case EntityType::SAVE_POINT: {
        SavePointData *save_data = static_cast<SavePointData*>(
          data.get());
        entity = make_unique<SavePoint>(*save_data);
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

  field->entity_queue.clear();
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

  actorBehavior();

  if (Game::state() != GameState::SLEEP) {
    for (unique_ptr<Entity> &entity : entities) {
      entity->update();
    }

    camera.follow(camera_target);
    eventProcessing();
  }

  if (panel_mode) {
    panelLogic();
    return;
  }

  if (sequence == nullptr) {
    return;
  }

  sequence->update();

  if (sequence->end_sequence) {
    sequence.reset();
  }
}

void FieldScene::panelLogic() {
  assert(panel != nullptr);
  panel->update();

  if (panel->terminate) {
    panelTermination();
  }
}

void FieldScene::panelTermination() {
  assert(panel != nullptr);

  if (panel->id == PanelID::DIALOG) {
    dialogHandling();
  }

  panel.reset();
  panel_mode = false;
}

void FieldScene::dialogHandling() {
  assert(panel != nullptr && panel->id == PanelID::DIALOG);

  DialogPanel *dialog = static_cast<DialogPanel*>(panel.get());
  if (dialog->selected != NULL && sequence != nullptr) {
    sequence->dialogHandling(*dialog->selected);
  }
}

void FieldScene::actorBehavior() {
  for (auto &event : *actor_handler.get()) {
    eventEvaluation(event);
  }

  for (Actor *actor : Actor::existing_actors) {
    actor->behavior();
  }

  actor_handler.clearEvents();
}

void FieldScene::eventEvaluation(unique_ptr<ActorEvent> &event) {
  for (Actor *actor : Actor::existing_actors) {
    if (event == nullptr) {
      break;
    }

    actor->evaluateEvent(event);
  }
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
      Game::saveSession(session.get());
      break;
    }
    case FieldEVT::OPEN_MENU: {
      PLOGD << "Event Detected: OpenMenuEvent";
      Game::openCampMenu(session.get());
      break;
    }
    case FieldEVT::OPEN_REST: {
      PLOGD << "Event Detected: OpenRestMenuEvent";
      Game::openRestMenu(session.get());
      break;
    }
    case FieldEVT::INIT_COMBAT: {
      PLOGD << "Event Detected: InitCombatEvent";

      Game::initCombat(session.get());
      sfx.play("combat_init");
      break;
    }
    case FieldEVT::INIT_COMBAT_FORCED: {
      PLOGD << "Event Detected: InitCombatFEvent";
      auto *event_data = static_cast<InitCombatFEvent*>(event.get());

      TroopID id = event_data->id;
      int reward = event_data->reward;

      Game::initCombat(session.get(), id, reward);
      sfx.play("combat_init");
      break;
    }
    case FieldEVT::GOTO_TITLE: {
      PLOGD << "Event Detected: GotoTitleEvent";
      Game::loadTitleScreen();
      break;
    }
    case FieldEVT::GAME_OVER: {
      PLOGD << "Event Detected: GameOverEvent";
      auto *event_data = static_cast<GameOverEvent*>(event.get());
      string reason = event_data->reason;

      Game::gameover(reason);
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
    case FieldEVT::MARK_AS_DEAD: {
      PLOGD << "Event detected: MarkAsDeadEvent";
      auto *event_data = static_cast<MarkAsDeadEvent*>(event.get());

      int object_id = event_data->object_id;

      PLOGI << "Marking enemy associated with Object ID: " << object_id <<
      " as dead.";
      markEnemyAsDead(object_id);
      break;
    }
    case FieldEVT::REVIVE_ENEMIES: {
      PLOGD << "Event detected: ReviveEnemiesEvent";
      reviveDeadEnemies();
      break;
    }
    case FieldEVT::CHANGE_SUPPLIES: {
      PLOGD << "Event detected: SetSuppliesEvent";
      auto *event_data = static_cast<SetSuppliesEvent*>(event.get());

      int value = event_data->value;

      PLOGI << "Changing value of 'supplies' to: " << value;
      session->supplies = value;
      break;
    }
    case FieldEVT::ADD_SUPPLIES: {
      PLOGD << "Event detected: AddSuppliesEvent";
      auto *event_data = static_cast<AddSuppliesEvent*>(event.get());

      int magnitude = event_data->magnitude;

      PLOGI << "Incrementing value of 'supplies' by: " << magnitude;
      session->supplies += magnitude;
      break;
    }
    case FieldEVT::CHANGE_PLR_LIFE: {
      PLOGD << "Event detected: SetPlrLifeEvent";
      auto *event_data = static_cast<SetLifeEvent*>(event.get());

      float value = event_data->value;

      PLOGI << "Changing the player's life expectancy to: " << value;
      session->player.life = value;
      break;
    }
    case FieldEVT::CHANGE_COM_LIFE: {
      PLOGD << "Event detected: SetComLifeEvent";
      auto *event_data = static_cast<SetLifeEvent*>(event.get());

      float value = event_data->value;

      PLOGI << "Changing the Companion's life Expectancy to: " << value;
      session->companion.life = value;
      break;
    }
    case FieldEVT::ADD_ITEM: {
      PLOGD << "Event detected: AddItemEvent";
      auto *event_data = static_cast<AddItemEvent*>(event.get());
      addItem(event_data->item);
      break;
    }
    case FieldEVT::REMOVE_ITEM: {
      PLOGD << "Event detected: RemoveItemEvent";
      auto *event_data = static_cast<RemoveItemEvent*>(event.get());
      removeItem(session.get(), event_data->item);
      break;
    }
    case FieldEVT::CLEAR_INV: {
      PLOGD << "Event detected: ClearInvEvent";
      clearInventory();
      break;
    }
    case FieldEVT::PLR_ADD_EFFECT:
    case FieldEVT::COM_ADD_EFFECT: {
      PLOGD << "Event detected: AddEffectEvent";
      auto *event_data = static_cast<AddEffectEvent*>(event.get());
      addStatusEffect(event_data->event_type, event_data->effect_id);
      break;
    }
    case FieldEVT::PLR_RM_EFFECT:
    case FieldEVT::COM_RM_EFFECT: {
      PLOGD << "Event detected: RemoveEffectEvent";
      auto *event_data = static_cast<RemoveEffectEvent*>(event.get());
      removeStatusEffect(event_data->event_type, event_data->effect_id);
      break;
    }
    case FieldEVT::OPEN_DIALOG: {
      PLOGD << "Event detected: OpenDialogEvent";
      auto *event_data = static_cast<OpenDialogEvent*>(event.get());
      Vector2 position = {97, 183};

      panel = make_unique<DialogPanel>(position, event_data->dialog, 
                                       event_data->end_prompt);
      panel_mode = true;
      break;
    }
    case FieldEVT::START_SEQUENCE: {
      PLOGD << "Event detected: StartSequenceEvent";
      auto *event_data = static_cast<StartSequenceEvent*>(event.get());
      SequenceID sequence_id = event_data->sequence;
      initSequence(sequence_id);
      break;
    }
  }
}

void FieldScene::initSequence(SequenceID sequence_id) {
  if (sequence != nullptr) {
    sequence.reset();
  }

  switch (sequence_id) { 
    #ifndef NDEBUG
    case SequenceID::DB_01:{
      sequence = make_unique<DBSequence01>();
      break;
    } 
    #endif // !NDEBUG
    case SequenceID::SAVE: {
      sequence = make_unique<SaveSequence>();
      break;
    }
    case SequenceID::REST: {
      sequence = make_unique<RestSequence>();
      break;
    }
  }

  assert(sequence != nullptr);
}

void FieldScene::addStatusEffect(FieldEVT type, StatusID effect_id) {
  Character *target;
  if (type == FieldEVT::PLR_ADD_EFFECT) {
    target = &session->player;
  }
  else if (type == FieldEVT::COM_ADD_EFFECT) {
    target = &session->companion;
  }
  else {
    PLOGE << "Invalid Event Type!";
    return;
  }

  PLOGD << "Target: '" << target->name << "'";
  if (target->status_count == target->status_limit) {
    PLOGI << "Target's status count is at it's limit!";
    return;
  }

  StatusID *empty_slot = NULL;
  for (int x = 0; x < target->status_limit; x++) {
    StatusID *status_slot = &target->status[x];

    if (*status_slot == effect_id) {
      PLOGE << "Target already has that status effect!";
      return;
    }

    if (empty_slot == NULL && *status_slot == StatusID::NONE) {
      PLOGD << "Found empty status slot.";
      empty_slot = status_slot;
    }
  }

  assert(empty_slot != NULL);
  PLOGI << "Afflicting target with status effect of ID: " << 
    static_cast<int>(effect_id); 
  *empty_slot = effect_id;

  target->status_count++;
  assert(target->status_count <= target->status_limit);
}

void FieldScene::removeStatusEffect(FieldEVT type, StatusID effect_id) {
  Character *target;
  if (type == FieldEVT::PLR_RM_EFFECT) {
    target = &session->player;
  }
  else if (type == FieldEVT::COM_RM_EFFECT) {
    target = &session->companion;
  }
  else {
    PLOGE << "Invalid Event Type!";
    return;
  }

  PLOGD << "Target: '" << target->name << "'";
  if (target->status_count == 0) {
    PLOGI << "Target doesn't have any status effects!";
    return;
  }

  for (int x = 0; x < target->status_limit; x++) {
    StatusID *status_slot = &target->status[x];

    if (*status_slot == effect_id) {
      PLOGI << "Cured target of status effect: " << 
        static_cast<int>(effect_id);

      *status_slot = StatusID::NONE;
      target->status_count--;
      assert(target->status_count >= 0);
      break;
    }
  }
}

void FieldScene::addItem(ItemID item) {
  assert(item != ItemID::NONE);
  int limit = session->item_limit;

  if (session->item_count == limit) {
    PLOGE << "Player's inventory is full!";
    return;
  }

  for (int index = 0; index < limit; index++) {
    ItemID *slot = &session->inventory[index];

    bool empty_slot = *slot == ItemID::NONE;
    if (empty_slot) {
      *slot = item;
      session->item_count++;
      PLOGI << "Added Item: " << static_cast<int>(*slot) <<
        " to player's inventory.";
      break;
    }
  }

  assert(session->item_count <= limit);
}

void FieldScene::removeItem(Session *session, ItemID item) {
  assert(item != ItemID::NONE);
  if (session->item_count == 0) {
    PLOGE << "Player's inventory is empty!";
    return;
  }

  int limit = session->item_limit;
  for (int index = 0; index < limit; index++) {
    ItemID *slot = &session->inventory[index];

    if (*slot == item) {
      PLOGI << "Removed Item: " << static_cast<int>(*slot) <<
        " from player's inventory.";
      *slot = ItemID::NONE;
      session->item_count--;
      break;
    }
  }

  assert(session->item_count >= 0);
}

void FieldScene::clearInventory() {
  int limit = session->item_limit;
  for (int index = 0; index < limit; index++) {
    ItemID *slot = &session->inventory[index];
    *slot = ItemID::NONE;
  }

  session->item_count = 0;
  PLOGI << "Cleared the player's inventory.";
}

void FieldScene::updateCommonData(int object_id, bool active) {
  int common_count = session->common_count;
  for (int x = 0; x < common_count; x++) {
    CommonData *data = &session->common[x];

    string map_name = data->map_name;

    if (map_name != session->map_name) {
      continue;
    }

    if (object_id == data->object_id) {
      data->active = active;
      return;
    }
  }

  PLOGE << "Failed to find common data!";
}

void FieldScene::markEnemyAsDead(int object_id) {
  int enemy_count = session->enemy_count;
  for (int x = 0; x < enemy_count; x++) {
    CommonData *data = &session->enemy[x];

    string map_name = data->map_name;

    if (map_name != session->map_name) {
      continue;
    }

    if (object_id == data->object_id) {
      data->active = false;
      return;
    }
  }

  PLOGE << "Failed to find enemy data associated with object id:" <<
  object_id;
}

void FieldScene::reviveDeadEnemies() {
  int enemy_count = session->enemy_count;
  for (int x = 0; x < enemy_count; x++) {
    CommonData *data = &session->enemy[x];

    if (!data->active) {
      data->active = true;
      PLOGD << "Object [ID:" << data->object_id << "] is now active.";
    }
  }
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
    field->draw();

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
    }

    #ifndef NDEBUG
    if (Game::debugInfo()) {
      for (unique_ptr<Entity> &entity : entities) {
        entity->drawDebug();
      }

      field->drawCollLines();
    } 
    #endif // !NDEBUG
  }
  EndMode2D();

  DrawTextureV(vignette, {0, 0}, WHITE);

  if (panel_mode) {
    panel->draw();
  }

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

#ifndef NDEBUG
void FieldScene::drawSessionInfo() {
  Font *font = &Game::sm_font;
  int text_size = Game::sm_font.baseSize;
  float base_x = 420;
  float y = 4;
  float spacing = 9;

  string location = TextFormat("Location: %s", session->map_name);
  Vector2 loc_pos = TextUtils::alignRight(location.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  string supplies = TextFormat("Supplies: %03i", session->supplies);
  Vector2 sup_pos = TextUtils::alignRight(supplies.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  string time = TextFormat("Playtime: %00.03f", Game::playtime());
  Vector2 time_pos = TextUtils::alignRight(time.c_str(), {base_x, y}, 
                                           *font, -3, 0);
  y += spacing;

  Player *player = &session->player;
  string plr_hp = TextFormat("%s Life: %02.00f / %02.00f", player->name, 
                             player->life, player->max_life);
  Vector2 php_pos = TextUtils::alignRight(plr_hp.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;



  Companion *companion = &session->companion;
  string com_hp = TextFormat("%s Life: %02.00f / %02.00f", 
                             companion->name, companion->life,
                             companion->max_life);
  Vector2 chp_pos = TextUtils::alignRight(com_hp.c_str(), {base_x, y}, 
                                          *font, -3, 0);
  y += spacing;

  string common = TextFormat("Common Count: %i / %i", 
                             session->common_count, session->common_limit);
  Vector2 common_pos = TextUtils::alignRight(common.c_str(), {base_x, y}, 
                                             *font, -3, 0);
  y += spacing;

  string enemy = TextFormat("Enemy Count: %i / %i",
                            session->enemy_count, session->enemy_limit);
  Vector2 enemy_pos = TextUtils::alignRight(enemy.c_str(), {base_x, y}, 
                                            *font, -3, 0);
  y += spacing;

  string pursue = TextFormat("Persuing Enemy: %i", 
                             EnemyActor::pursuing_enemy);
  Vector2 per_pos = TextUtils::alignRight(pursue.c_str(), {base_x, y}, 
                                          *font, -3, 0);


  DrawTextEx(*font, location.c_str(), loc_pos, text_size, -3, GREEN);
  DrawTextEx(*font, supplies.c_str(), sup_pos, text_size, -3, GREEN);
  DrawTextEx(*font, time.c_str(), time_pos, text_size, -3, GREEN);
  DrawTextEx(*font, plr_hp.c_str(), php_pos, text_size, -3, GREEN);
  DrawTextEx(*font, com_hp.c_str(), chp_pos, text_size, -3, GREEN);
  DrawTextEx(*font, common.c_str(), common_pos, text_size, -3, GREEN);
  DrawTextEx(*font, enemy.c_str(), enemy_pos, text_size, -3, GREEN);
  DrawTextEx(*font, pursue.c_str(), per_pos, text_size, -3, GREEN);
}
#endif // !NDEBUG
