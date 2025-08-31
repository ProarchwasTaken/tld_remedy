#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "combat/system/stage.h"
#include "combat/system/camera.h"
#include "combat/system/evt_handler.h"
#include "combat/hud/life.h"
#include "combat/combatants/party/mary.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void initializeCombatants();

  void update() override;
  void eventProcessing();
  void eventHandling(std::unique_ptr<CombatEvent> &event);

  void deleteEntity(int entity_id);
  void endCombatProcedure();
  void updatePlrStats(Player *plr_data);

  void draw() override;
  void drawDebugInfo();
private:
  CombatHandler evt_handler;
  CombatCamera camera;
  CombatStage stage;
  Texture debug_overlay;
  bool game_over = false;

  Mary *player;
  LifeHud plr_hud = LifeHud({34, 215});

  std::vector<std::unique_ptr<Entity>> entities;
  Session *session;
};
