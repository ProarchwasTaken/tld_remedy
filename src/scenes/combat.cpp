#include <cassert>
#include <cstddef>
#include <memory>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "base/enemy.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "scenes/field.h"
#include "combat/system/evt_handler.h"
#include "combat/hud/life.h"
#include "combat/hud/enemy.h"
#include "combat/hud/combo.h"
#include "combat/hud/toasts.h"
#include "combat/hud/cmd_plr.h"
#include "combat/hud/cmd_assist.h"
#include "combat/hud/cmd_item.h"
#include "combat/entities/dmg_number.h"
#include "combat/entities/status_text.h"
#include "combat/entities/afterimage.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/erwin.h"
#include "combat/combatants/enemy/dummy.h"
#include "combat/combatants/enemy/servant.h"
#include "scenes/combat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::string, std::vector;
bool combatAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2);
SpriteAtlas CombatScene::cmd_atlas("hud", "hud_command");

CombatScene::CombatScene(Session *session) {
  PLOGI << "Loading the combat scene.";
  assert(session != NULL);
  float start_time = GetTime();

  scene_id = SceneID::COMBAT;
  this->session = session;

  stage.loadStage("debug");
  initializeCombatants();

  combo_hud = make_unique<ComboHud>((Vector2){24, 27});
  toasts = make_unique<CombatToasts>((Vector2){24, 21});

  PLOGD << "Sorting Combat entities in their intended order.";
  std::sort(entities.begin(), entities.end(), combatAlgorithm);

  #ifndef NDEBUG
  debug_overlay = LoadTexture("graphics/stages/debug_overlay.png"); 
  #endif // !NDEBUG

  PLOGI << "Player Party: " << PartyMember::memberCount();
  PLOGI << "Enemies present: " << Enemy::memberCount(); 

  float end_time = GetTime();
  PLOGD << "Load Time: " << end_time - start_time;
}

CombatScene::~CombatScene() {
  plr_hud.reset();
  plr_cmd_hud.reset();

  com_hud.reset();

  enemy_hud.reset();
  item_hud.reset();
  combo_hud.reset();
  toasts.reset();

  Entity::clear(entities);

  UnloadTexture(debug_overlay);
  assert(Combatant::existing_combatants.empty());
  PLOGI << "Unloaded the Combat scene.";
}

void CombatScene::initializeCombatants() {
  PLOGI << "Initializing combatants.";
  initializePlayer();
  initializeCompanion();

  enemy_hud = make_unique<EnemyHud>((Vector2){409, 16});
  enemy_hud->assign(player, companion);

  item_hud = make_unique<ItemCmdHud>((Vector2){350, 222});
  item_hud->assign(player, companion, session);

  EnemyTroop troop = DBTroop1();
  assert(troop.id != TroopID::INVALID && !troop.enemies.empty());
  initializeTroop(&troop);
}

void CombatScene::initializePlayer() {
  PLOGI << "Initializing Player Combatant.";
  auto player = make_unique<Mary>(&session->player);
  this->player = player.get();

  plr_hud = make_unique<LifeHud>((Vector2){34, 215});
  plr_hud->assign(this->player);

  plr_cmd_hud = make_unique<PlayerCmdHud>((Vector2){350, 178});
  plr_cmd_hud->assign(this->player);
  entities.push_back(std::move(player));
}

void CombatScene::initializeCompanion() {
  PLOGI << "Initializing Companion Combatant.";
  unique_ptr<PartyMember> companion;
  Companion *data = &session->companion;

  switch (data->companion_id) {
    case CompanionID::ERWIN: {
      companion = make_unique<Erwin>(data, player);
    }
  }

  assert(companion != nullptr);
  this->companion = companion.get();

  com_hud = make_unique<LifeHud>((Vector2){154, 215});
  com_hud->assign(this->companion);

  assist_hud = make_unique<AssistCmdHud>((Vector2){274, 200});
  assist_hud->assign(this->companion);

  entities.push_back(std::move(companion));
}

void CombatScene::initializeTroop(EnemyTroop *troop) {
  PLOGI << "Initializing Enemy Troop of ID: " << 
  static_cast<int>(troop->id);

  for (EnemyData &data : troop->enemies) {
    unique_ptr<Enemy> enemy = createEnemy(data);

    assert(enemy != nullptr);
    #ifndef NDEBUG
    if (dummy == NULL && enemy->id == EnemyID::DUMMY) {
      dummy = static_cast<Dummy*>(enemy.get());
    } 
    #endif // NDEBUG

    entities.push_back(std::move(enemy));
  }
}

unique_ptr<Enemy> CombatScene::createEnemy(EnemyData &data) {
  EnemyID id = data.enemy;
  Vector2 position = data.position;
  Direction direction = data.direction;

  switch (id) {
    case EnemyID::DUMMY: {
      return make_unique<Dummy>(position, direction);
    }
    case EnemyID::SERVANT: {
      return make_unique<Servant>(position, direction);
    }
    default: {
      return nullptr;
    }
  }
}

void CombatScene::update() {
  #ifndef NDEBUG
  debugKeybinds(); 
  #endif // !NDEBUG

  combatantBehavior();

  if (Game::state() == GameState::READY) {
    stage.update();

    for (unique_ptr<Entity> &entity : entities) {
      entity->update();
    }

    camera.update(player);
    black_bars.update(&camera);

    updateHud();
    eventProcessing();
  }
}

void CombatScene::combatantBehavior() {
  for (Combatant *combatant : Combatant::existing_combatants) {
    combatant->behavior();
  }

  EventPool<CombatantEvent> *event_pool = CombatantHandler::get();
  for (auto &event : *event_pool) {
    eventEvaluation(event);
  }

  cbt_handler.clearEvents();
}

void CombatScene::eventEvaluation(unique_ptr<CombatantEvent> &event) {
  if (event == nullptr) {
    return;
  }

  for (Combatant *combatant : Combatant::existing_combatants) {
    combatant->evaluateEvent(event);
  }

  plr_hud->evaluateEvent(event);
  com_hud->evaluateEvent(event);
  enemy_hud->evaluateEvent(event);
  combo_hud->evaluateEvent(event);
}

void CombatScene::updateHud() {
  plr_hud->update();
  plr_cmd_hud->update();

  com_hud->update();
  assist_hud->update();

  enemy_hud->update();
  item_hud->update();
  combo_hud->update();
}

void CombatScene::eventProcessing() {
  EventPool<CombatEvent> *event_pool = evt_handler.get();
  if (!event_pool->empty()) {
    PLOGI << "Combat Events raised: " << event_pool->size();
    for (auto &event : *event_pool) {
      eventHandling(event);
    }

    evt_handler.clear();
  }

  game_over = player == NULL;
  if (game_over || Enemy::memberCount() == 0) {
    endCombatProcedure();
  }

  cbt_handler.transferEvents();
}

void CombatScene::eventHandling(unique_ptr<CombatEvent> &event) {
  int count = entities.size();

  switch (event->event_type) { 
    case CombatEVT::DELETE_ENTITY: {
      PLOGD << "Event detected: DeleteEntityCB";
      auto *event_data = static_cast<DeleteEntityCB*>(event.get());

      int entity_id = event_data->entity_id;

      PLOGI << "Attempting to delete Entity [ID: " << entity_id << "]";
      deleteEntity(entity_id);
      break;
    }
    case CombatEVT::CREATE_DMG_NUM: {
      PLOGD << "Event detected: CreateDmgNumCB";
      auto *event_data = static_cast<CreateDmgNumCB*>(event.get());

      Combatant *target = event_data->target;
      DamageType damage_type = event_data->damage_type;
      float damage_taken = event_data->damage_taken;

      dmgNumberHandling(target, damage_type, damage_taken);
      break;
    }
    case CombatEVT::CREATE_STAT_TXT: {
      PLOGD << "Event detected: CreateStatTxtCB";
      auto *event_data = static_cast<CreateStatTxtCB*>(event.get());

      Combatant *target = event_data->target;
      string text = event_data->text;
      Color color = event_data->color;

      auto status_txt = make_unique<StatusText>(target, text, color);
      entities.push_back(std::move(status_txt));
      break;
    }
    case CombatEVT::CREATE_AFTERIMAGE: {
      PLOGD << "Event detected: CreateAfterImageCB";
      auto *event_data = static_cast<CreateAfterImgCB*>(event.get());

      auto afterimage = make_unique<AfterImage>(event_data);
      entities.push_back(std::move(afterimage));
      break;
    }
    case CombatEVT::OPEN_ITEM_HUD: {
      PLOGD << "Event detected: OpenItemHud";
      item_hud->enable();
      break;
    }
    case CombatEVT::REMOVE_ITEM: {
      PLOGD << "Event detected: RemoveItemCB";
      auto *event_data = static_cast<RemoveItemCB*>(event.get());
      FieldScene::removeItem(session, event_data->item);
      break;
    }
    case CombatEVT::START_TOAST: {
      PLOGD << "Event detected: StartToastCB";
      auto *event_data = static_cast<StartToastCB*>(event.get());
      toasts->startToast(event_data->toast_id);
      break;
    }
  }

  int new_count = entities.size();
  if (count != new_count) {
    PLOGD << "Sorting Combat entities in their intended order.";
    std::sort(entities.begin(), entities.end(), combatAlgorithm);
  }
}

void CombatScene::dmgNumberHandling(Combatant *target, 
                                    DamageType damage_type,
                                    float damage_taken) 
{
  for (auto &entity : entities) {
    if (entity->entity_type != EntityType::DMG_NUMBER) {
      continue;
    }

    DamageNumber *dmg_num = static_cast<DamageNumber*>(entity.get());
    
    if (target != dmg_num->target) {
      continue;
    }

    if (damage_type == dmg_num->type) {
      dmg_num->incrementValue(damage_taken);
      return;
    }
  }

  PLOGD << "Proceeding to create new Damage Number.";
  auto dmg_num = make_unique<DamageNumber>(target, damage_type, 
                                           damage_taken);
  entities.push_back(std::move(dmg_num));
}

void CombatScene::deleteEntity(int entity_id) {
  vector<unique_ptr<Entity>> temporary;

  for (auto &entity : entities) {
    if (entity->entity_id != entity_id) {
      unique_ptr<Entity> temp_entity = nullptr;
      entity.swap(temp_entity);

      temporary.push_back(std::move(temp_entity));
      continue;
    }
    PLOGD << "Found Entity [ID: " << entity_id << "]";

    // Remove this Later!
    if (dummy == entity.get()) dummy = NULL;

    if (player == entity.get()) {
      player = NULL;
      plr_hud->assign(player);
      plr_cmd_hud->assign(player);
    }
    else if (companion == entity.get()) {
      companion = NULL;
      com_hud->assign(companion);
      assist_hud->assign(companion);
    }

    entity.reset();
  }

  entities.clear();
  temporary.swap(entities);
}

void CombatScene::endCombatProcedure() {
  Player *plr_data = &session->player;
  Companion *com_data = &session->companion;

  PLOGD << "Updating player attributes.";
  updatePartyAttr(player, plr_data);

  PLOGD << "Updating companion attributes.";
  updatePartyAttr(companion, com_data);

  Game::returnToField();
}

void CombatScene::updatePartyAttr(PartyMember *member, Character *data) 
{
  if (member == NULL) {
    PLOGD << "Combatant is assumed to be dead. Setting Life to 1.";
    data->life = 1;
    return;
  }

  member->depleteInstant();
  float life = std::ceilf(member->life); 
  data->life = Clamp(life, 0, member->max_life);

  StatusID status[STATUS_LIMIT] = {
    StatusID::NONE, 
    StatusID::NONE, 
    StatusID::NONE
  };

  int index = 0;
  data->status_count = 0;
  int limit = data->status_limit;

  for (unique_ptr<StatusEffect> &effect : member->status) {
    if (effect->isPersistent()) {
      status[index] = effect->id;
      index++;
      data->status_count++;
      assert(data->status_count <= limit);
    }
  }

  std::copy(status, status + 3, data->status);
}

void CombatScene::draw() {
  bool debug_info = Game::debugInfo();

  BeginMode2D(camera);
  {
    stage.drawBackground();
    black_bars.draw();

    #ifndef NDEBUG
    if (debug_info) DrawTextureV(debug_overlay, {-512, 0}, WHITE);
    #endif // !NDEBUG

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
    }
    
    stage.drawOverlay();

    #ifndef NDEBUG
    if (debug_info) {
      for (unique_ptr<Entity> &entity : entities) {
        entity->drawDebug();
      }
    } 
    #endif // !NDEBUG
  }
  EndMode2D();

  drawHud();

  #ifndef NDEBUG
  if (debug_info) drawDebugInfo();
  #endif // !NDEBUG
}

void CombatScene::drawHud() {
  plr_hud->draw();
  plr_cmd_hud->draw();

  com_hud->draw();
  assist_hud->draw();

  enemy_hud->draw();
  item_hud->draw();
  combo_hud->draw();
  toasts->draw();
}

bool combatAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2) {
  if (e1->entity_type != e2->entity_type) {
    return e1->entity_type > e2->entity_type;
  }
  
  if (e1->entity_type != EntityType::COMBATANT) {
    return e1->entity_id < e2->entity_id;
  }

  Combatant *c1 = static_cast<Combatant*>(e1.get());
  Combatant *c2 = static_cast<Combatant*>(e2.get());

  if (c1->state != c2->state) {
    return c1->state > c2->state;
  }

  if (c1->team != c2->team){
    return c1->team > c2->team;
  }
  
  if (c1->team == CombatantTeam::PARTY) {
    PartyMember *p1 = static_cast<PartyMember*>(c1);
    PartyMember *p2 = static_cast<PartyMember*>(c2);

    return p1->important < p2->important;
  }
  else {
    return c1->entity_id < c2->entity_id;
  }
}

#ifndef NDEBUG
void CombatScene::debugKeybinds() {
  if (dummy != NULL && IsKeyPressed(KEY_SLASH)) {
    dummy->attack();
  }
  else if (companion != NULL && IsKeyPressed(KEY_F4)) {
    bool enabled = companion->isEnabled();
    companion->setEnabled(!enabled);
  }
  else if (player != NULL && IsKeyPressed(KEY_F5)) {
    float threshold = 0.5;
    float magnitude = player->life * threshold;
    player->increaseTenacity(magnitude, threshold);
  }
  else if (companion != NULL && IsKeyPressed(KEY_F6)) {
    float threshold = 0.5;
    float magnitude = companion->life * threshold;
    companion->increaseTenacity(magnitude, threshold);
  }
  else if (IsKeyPressed(KEY_F7)) {
    endCombatProcedure();
  }
}

void CombatScene::drawDebugInfo() {
  Font *font = &Game::sm_font;
  int text_size = font->baseSize;
  
  drawPartyStats(player, {6, 4}, font, text_size);
  if (companion != NULL) {
    drawPartyStats(companion, {128, 4}, font, text_size);
  }

  drawDebugCombo(font, text_size);
}

void CombatScene::drawPartyStats(PartyMember *member, Vector2 position, 
                                 Font *font, int text_size) 
{
  if (member == NULL) {
    return;
  }

  float spacing = 9;

  string text = "Name: " + member->name;
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Position: (%00.01f, %00.01f)", member->position.x, 
                    member->position.y);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Life: %02.02f/%02.02f", member->life,
                    member->max_life);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Morale: %02.02f/%02.02f/%02.02f", 
                    member->morale, member->init_morale,
                    member->max_morale);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Tenacity: %01.02f/%01.02f", member->tenacity,
                    member->tp_threshold);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Exhaustion: %02.02f", member->exhaustion);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Speed Multiplier: %01.02f", 
                    member->speed_multiplier);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Recovery: %01.02f", member->recovery);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y += spacing;

  text = TextFormat("Resilience: %01.02f", member->resilience);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
  position.y = 228;

  if (member->important) {
    return;
  }

  int goal_id;
  switch (member->id) {
    case PartyMemberID::ERWIN: {
      Erwin *erwin = static_cast<Erwin*>(member);
      goal_id = static_cast<int>(erwin->ai_goal);
    }
    default: {
      assert(member->id != PartyMemberID::MARY);
    }
  }

  text = TextFormat("AI Goal: %i", goal_id);
  DrawTextEx(*font, text.c_str(), position, text_size, -3, GREEN);
}

void CombatScene::drawDebugCombo(Font *font, int text_size) {
  string combo = TextFormat("True Combo: %02i", Enemy::comboCount());
  Vector2 combo_pos = {6, 228};

  DrawTextEx(*font, combo.c_str(), combo_pos, text_size, -3, GREEN);
}
#endif // !NDEBUG
