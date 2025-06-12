#include <cassert>
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "data/session.h"
#include "scenes/combat.h"
#include <plog/Log.h>


CombatScene::CombatScene(Session *session) {
  PLOGI << "Loading the combat scene.";
  assert(session != NULL);
  scene_id = SceneID::COMBAT;

  player = &session->player;
  companion = &session->companion;
}

CombatScene::~CombatScene() {
  Entity::clear(entities);

  assert(Combatant::existing_combatants.empty());
  PLOGI << "Unloaded the Combat scene.";
}

void CombatScene::update() {

}

void CombatScene::draw() {

}
