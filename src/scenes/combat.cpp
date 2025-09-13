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
#include "system/sprite_atlas.h"
#include "combat/system/evt_handler.h"
#include "combat/entities/dmg_number.h"
#include "combat/entities/status_text.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/erwin.h"
#include "combat/combatants/enemy/dummy.h"
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

  #ifndef NDEBUG
  debug_overlay = LoadTexture("graphics/stages/debug_overlay.png"); 
  #endif // !NDEBUG

  PLOGI << "Player Party: " << PartyMember::memberCount();
  PLOGI << "Enemies present: " << Enemy::memberCount(); 

  float end_time = GetTime();
  PLOGD << "Load Time: " << end_time - start_time;
}

CombatScene::~CombatScene() {
  Entity::clear(entities);

  UnloadTexture(debug_overlay);
  assert(Combatant::existing_combatants.empty());
  PLOGI << "Unloaded the Combat scene.";
}

void CombatScene::initializeCombatants() {
  PLOGI << "Initializing combatants.";
  initializePlayer();
  initializeCompanion();

  // TODO: Don't forget to remove this when the time comes!
  auto dummy = make_unique<Dummy>((Vector2){128, 152}, LEFT);
  this->dummy = dummy.get();
  entities.push_back(std::move(dummy));

  dummy = make_unique<Dummy>((Vector2){-256, 152}, RIGHT);
  entities.push_back(std::move(dummy));

  dummy = make_unique<Dummy>((Vector2){-288, 152}, RIGHT);
  entities.push_back(std::move(dummy));
}

void CombatScene::initializePlayer() {
  PLOGI << "Initializing Player Combatant.";
  auto player = make_unique<Mary>(&session->player);
  this->player = player.get();

  plr_hud.assign(this->player);
  plr_cmd_hud.assign(this->player);
  entities.push_back(std::move(player));
}

void CombatScene::initializeCompanion() {
  PLOGI << "Initializing Companion Combatant.";
  unique_ptr<PartyMember> companion;
  Companion *data = &session->companion;

  switch (data->companion_id) {
    case CompanionID::ERWIN: {
      companion = make_unique<Erwin>(data);
    }
  }

  assert(companion != nullptr);
  this->companion = companion.get();
  entities.push_back(std::move(companion));
}

void CombatScene::update() {
  // Remove this Later!
  if (dummy != NULL && IsKeyPressed(KEY_SLASH)) {
    dummy->attack();
  }

  for (Combatant *combatant : Combatant::existing_combatants) {
    combatant->behavior();
  }
  plr_hud.behavior();
  cbt_handler.clearEvents();

  if (Game::state() == GameState::READY) {
    stage.update();

    for (unique_ptr<Entity> &entity : entities) {
      entity->update();
    }

    camera.update(player);

    plr_hud.update();
    plr_cmd_hud.update();

    eventProcessing();
  }
}

void CombatScene::eventProcessing() {
  game_over = player->state == DEAD && player->deathClock() == 1.0;
  if (game_over || Enemy::memberCount() == 0) {
    endCombatProcedure();
  }

  EventPool<CombatEvent> *event_pool = evt_handler.get();
  if (!event_pool->empty()) {
    PLOGI << "Combat Events raised: " << event_pool->size();
    for (auto &event : *event_pool) {
      eventHandling(event);
    }

    evt_handler.clear();
  }

  cbt_handler.transferEvents();
}

void CombatScene::eventHandling(unique_ptr<CombatEvent> &event) {
  switch (event->event_type) { 
    case CombatEVT::DELETE_ENTITY: {
      PLOGD << "Event detected: DeleteEntityEvent";
      auto *event_data = static_cast<DeleteEntityCB*>(event.get());

      int entity_id = event_data->entity_id;

      PLOGI << "Attempting to delete Entity [ID: " << entity_id << "]";
      deleteEntity(entity_id);
      break;
    }
    case CombatEVT::CREATE_DMG_NUM: {
      PLOGD << "Event detected: CreateDmgNumEvent";
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
    }
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
    if (entity->entity_id == entity_id) {
      PLOGD << "Found Entity [ID: " << entity_id << "]";
      // Remove this Later!
      if (dummy == entity.get()) dummy = NULL;

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

void CombatScene::endCombatProcedure() {
  Player *plr_data = &session->player;
  if (!game_over) {
    PLOGI << "All enemies are defeated!";
    updatePlrStats(plr_data);
  }
  else {
    PLOGI << "The party leader has died...";
    plr_data->life = 1;
  }

  Game::endCombat();
}

void CombatScene::updatePlrStats(Player *plr_data) {
  player->depleteInstant();
  float life = std::ceilf(player->life); 
  plr_data->life = Clamp(life, 0, player->max_life);

  StatusID status[STATUS_LIMIT] = {
    StatusID::NONE, 
    StatusID::NONE, 
    StatusID::NONE
  };

  int index = 0;
  plr_data->status_count = 0;
  int limit = plr_data->status_limit;

  for (unique_ptr<StatusEffect> &effect : player->status) {
    if (effect->isPersistant()) {
      status[index] = effect->id;
      index++;
      plr_data->status_count++;
      assert(plr_data->status_count <= limit);
    }
  }

  std::copy(status, status + 3, plr_data->status);
}

void CombatScene::draw() {
  bool debug_info = Game::debugInfo();
  std::sort(entities.begin(), entities.end(), combatAlgorithm);

  BeginMode2D(camera);
  {
    stage.drawBackground();

    #ifndef NDEBUG
    if (debug_info) DrawTextureV(debug_overlay, {-512, 0}, WHITE);
    #endif // !NDEBUG

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
    }
    
    stage.drawOverlay();

    #ifndef NDEBUG
    if (Game::debugInfo()) {
      for (unique_ptr<Entity> &entity : entities) {
        entity->drawDebug();
      }
    } 
    #endif // !NDEBUG
  }
  EndMode2D();

  plr_hud.draw();
  plr_cmd_hud.draw();

  #ifndef NDEBUG
  if (debug_info) drawDebugInfo();
  #endif // !NDEBUG
}

bool combatAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2) {
  if (e1->entity_type != e2->entity_type) {
    return e1->entity_type > e2->entity_type;
  }
  
  if (e1->entity_type != EntityType::COMBATANT) {
    return e1->position.y < e2->position.y;
  }

  Combatant *c1 = static_cast<Combatant*>(e1.get());
  Combatant *c2 = static_cast<Combatant*>(e2.get());

  if (c1->state != c2->state) {
    return c1->state > c2->state;
  }
  else {
    return c1->team > c2->team;
  }
}

void CombatScene::drawDebugInfo() {
  Font *font = &Game::sm_font;
  int text_size = font->baseSize;
  
  drawPartyStats(player, {6, 4}, font, text_size);
  drawPartyStats(companion, {128, 4}, font, text_size);
  drawDebugCombo(font, text_size);
}

void CombatScene::drawPartyStats(PartyMember *member, Vector2 position, 
                                 Font *font, int text_size) 
{
  if (member == NULL) {
    return;
  }

  float spacing = 9;

  string name = "Name: " + member->name;
  Vector2 n_pos = position;
  position.y += spacing;

  string state = TextFormat("State: %i", member->state);
  Vector2 s_pos = position;
  position.y += spacing;

  string hp = TextFormat("Life: %02.02f/%02.02f", member->life,
                         member->max_life);
  Vector2 hp_pos = position;
  position.y += spacing;

  string mp = TextFormat("Morale: %02.02f/%02.02f/%02.02f", 
                         member->morale, member->init_morale,
                         member->max_morale);
  Vector2 mp_pos = position;
  position.y += spacing;

  string ex = TextFormat("Exhaustion: %02.02f", member->exhaustion);
  Vector2 ex_pos = position;
  position.y += spacing;


  DrawTextEx(*font, name.c_str(), n_pos, text_size, -3, GREEN);
  DrawTextEx(*font, state.c_str(), s_pos, text_size, -3, GREEN);
  DrawTextEx(*font, hp.c_str(), hp_pos, text_size, -3, GREEN);
  DrawTextEx(*font, mp.c_str(), mp_pos, text_size, -3, GREEN);
  DrawTextEx(*font, ex.c_str(), ex_pos, text_size, -3, GREEN);
}

void CombatScene::drawDebugCombo(Font *font, int text_size) {
  string combo = TextFormat("True Combo: %02i", Enemy::comboCount());
  Vector2 combo_pos = {6, 49};

  DrawTextEx(*font, combo.c_str(), combo_pos, text_size, -3, GREEN);
}
