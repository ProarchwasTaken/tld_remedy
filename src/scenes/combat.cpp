#include <memory>
#include <cassert>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "data/session.h"
#include "utils/camera.h"
#include "combat/combatants/player.h"
#include "scenes/combat.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


CombatScene::CombatScene(Session *session) {
  PLOGI << "Loading the combat scene.";
  assert(session != NULL);
  scene_id = SceneID::COMBAT;

  camera = CameraUtils::setupCombat();

  auto player = make_unique<PlayerCombatant>(&session->player);
  entities.push_back(std::move(player));
}

CombatScene::~CombatScene() {
  Entity::clear(entities);

  assert(Combatant::existing_combatants.empty());
  PLOGI << "Unloaded the Combat scene.";
}

void CombatScene::update() {
  if (IsKeyPressed(KEY_BACKSPACE)) {
    Game::endCombat();
  }
}

void CombatScene::draw() {
  BeginMode2D(camera);
  {
    for (unique_ptr<Entity> &entity : entities) {
      entity->draw();
      entity->drawDebug();
    }
  }
  EndMode2D();
}
