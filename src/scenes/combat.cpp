#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/enemy.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "combat/system/evt_handler.h"
#include "combat/entities/dmg_number.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/enemy/dummy.h"
#include "scenes/combat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::string, std::vector;
bool combatAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2);

CombatScene::CombatScene(Session *session) {
  PLOGI << "Loading the combat scene.";
  assert(session != NULL);
  scene_id = SceneID::COMBAT;
  this->session = session;

  stage.loadStage("debug");

  auto player = make_unique<Mary>(&session->player);
  this->player = player.get();
  entities.push_back(std::move(player));

  // TODO: Don't forget to remove this when the time comes!
  auto dummy = make_unique<Dummy>((Vector2){128, 152}, LEFT);
  this->dummy = dummy.get();
  entities.push_back(std::move(dummy));

  #ifndef NDEBUG
  debug_overlay = LoadTexture("graphics/stages/debug_overlay.png"); 
  #endif // !NDEBUG

  PLOGI << "Player Party: " << PartyMember::memberCount();
  PLOGI << "Enemies present: " << Enemy::memberCount(); 
}

CombatScene::~CombatScene() {
  Entity::clear(entities);

  UnloadTexture(debug_overlay);
  assert(Combatant::existing_combatants.empty());
  PLOGI << "Unloaded the Combat scene.";
}

void CombatScene::update() {
  // TODO: Don't forget to remove this when the time comes!
  if (IsKeyPressed(KEY_SLASH)) {
    dummy->attack();
  }
  // TODO: This too!
  if (IsKeyPressed(KEY_O)) {
    player->increaseExhaustion(7.5);
  }

  for (Combatant *combatant : Combatant::existing_combatants) {
    combatant->behavior();
  }

  if (Game::state() == GameState::SLEEP) {
    return;
  }

  stage.update();

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  camera.update(player);
  eventProcessing();
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

      auto dmg_num = make_unique<DamageNumber>(target, damage_type, 
                                               damage_taken);
      entities.push_back(std::move(dmg_num));
      break;
    }
  }
}

void CombatScene::deleteEntity(int entity_id) {
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

void CombatScene::endCombatProcedure() {
  Player *plr_data = &session->player;
  if (!game_over) {
    PLOGI << "All enemies are defeated!";
    plr_data->life = player->life;
  }
  else {
    PLOGI << "The party leader has died...";
    plr_data->life = 1;
  }

  Game::endCombat();
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
  
  Vector2 position = {6, 4};
  float spacing = 9;

  string p_name = "Player Name: " + player->name;
  Vector2 pn_pos = position;
  position.y += spacing;

  string p_state = TextFormat("State: %i", player->state);
  Vector2 ps_pos = position;
  position.y += spacing;

  string p_hp = TextFormat("Life: %02.02f/%02.02f", player->life,
                           player->max_life);
  Vector2 php_pos = position;
  position.y += spacing;

  string p_mp = TextFormat("Morale: %02.02f/%02.02f/%02.02f",
                           player->morale, player->init_morale,
                           player->max_morale);
  Vector2 pmp_pos = position;
  position.y += spacing;

  string p_ex = TextFormat("Exhaustion: %02.02f", player->exhaustion);
  Vector2 pex_pos = position;
  position.y += spacing;

  string combo = TextFormat("True Combo: %02i", Enemy::comboCount());
  Vector2 combo_pos = position;

  DrawTextEx(*font, p_name.c_str(), pn_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_state.c_str(), ps_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_hp.c_str(), php_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_mp.c_str(), pmp_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_ex.c_str(), pex_pos, text_size, -3, GREEN);
  DrawTextEx(*font, combo.c_str(), combo_pos, text_size, -3, GREEN);
}
