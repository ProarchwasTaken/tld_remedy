#include <cassert>
#include <memory>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/session.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/nme_dummy.h"
#include "scenes/combat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique, std::string;


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
  auto dummy = make_unique<DummyEnemy>((Vector2){256, 152}, LEFT);
  this->dummy = dummy.get();

  entities.push_back(std::move(dummy));
  #endif // !NDEBUG

  PLOGI << "Player Party: " << PartyMember::memberCount();
  PLOGI << "Enemies present: " << Combatant::enemyCount(); 
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

  for (unique_ptr<Entity> &entity : entities) {
    entity->update();
  }

  camera.update(player);
}

void CombatScene::draw() {
  bool debug_info = Game::debugInfo();

  BeginMode2D(camera);
  {
    stage.drawBackground();

    #ifndef NDEBUG
    if (debug_info) DrawTextureV(debug_overlay, {-512, 0}, WHITE);
    #endif // !NDEBUG

    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
      entity->drawDebug();
    }
    
    stage.drawOverlay();
  }
  EndMode2D();

  #ifndef NDEBUG
  if (debug_info) drawDebugInfo();
  #endif // !NDEBUG
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
