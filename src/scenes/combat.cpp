#include <cassert>
#include <memory>
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
#include "combat/combatants/party/mary.h"
#include "combat/combatants/enemy/dummy.h"
#include "scenes/combat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::string;
bool combatAlgorithm(unique_ptr<Entity> &e1, unique_ptr<Entity> &e2);

CombatScene::CombatScene(Session *session) {
  PLOGI << "Loading the combat scene.";
  assert(session != NULL);
  scene_id = SceneID::COMBAT;

  stage.loadStage("debug");

  auto player = make_unique<Mary>(&session->player);
  this->player = player.get();

  entities.push_back(std::move(player));

  #ifndef NDEBUG
  debug_overlay = LoadTexture("graphics/stages/debug_overlay.png"); 
  auto dummy = make_unique<Dummy>((Vector2){256, 152}, LEFT);
  this->dummy = dummy.get();

  entities.push_back(std::move(dummy));
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
  if (IsKeyPressed(KEY_BACKSPACE)) {
    Game::endCombat();
  }
  if (IsKeyPressed(KEY_P)) {
    dummy->attack();
  }

  for (Combatant *combatant : Combatant::existing_combatants) {
    combatant->behavior();
  }

  if (Game::state() == GameState::SLEEP) {
    return;
  }

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  camera.update(player);
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

  DrawTextEx(*font, p_name.c_str(), pn_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_state.c_str(), ps_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_hp.c_str(), php_pos, text_size, -3, GREEN);
  DrawTextEx(*font, p_mp.c_str(), pmp_pos, text_size, -3, GREEN);
}
