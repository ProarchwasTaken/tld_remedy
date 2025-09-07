#pragma once
#include <vector>
#include <memory>
#include "enums.h"
#include "base/scene.h"
#include "base/entity.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "system/sprite_atlas.h"
#include "combat/system/stage.h"
#include "combat/system/camera.h"
#include "combat/system/evt_handler.h"
#include "combat/hud/life.h"
#include "combat/hud/cmd_plr.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/enemy/dummy.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void initializeCombatants();

  void update() override;
  void eventProcessing();
  void eventHandling(std::unique_ptr<CombatEvent> &event);

  void dmgNumberHandling(Combatant *target, DamageType damage_type,
                         float damage_taken);
  void deleteEntity(int entity_id);
  void endCombatProcedure();
  void updatePlrStats(Player *plr_data);

  void draw() override;
  void drawDebugInfo();

  static SpriteAtlas cmd_atlas;
private:
  CombatHandler evt_handler;
  CombatCamera camera;
  CombatStage stage;
  Texture debug_overlay;
  bool game_over = false;

  Mary *player;
  LifeHud plr_hud = LifeHud({34, 215});
  PlayerCmdHud plr_cmd_hud = PlayerCmdHud({350, 178});

  // Remove this later!
  Dummy *dummy = NULL;

  std::vector<std::unique_ptr<Entity>> entities;
  Session *session;
};
