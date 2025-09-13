#pragma once
#include <raylib.h>
#include <vector>
#include <memory>
#include "enums.h"
#include "base/scene.h"
#include "base/entity.h"
#include "base/party_member.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "system/sprite_atlas.h"
#include "combat/system/stage.h"
#include "combat/system/camera.h"
#include "combat/system/evt_handler.h"
#include "combat/system/cbt_handler.h"
#include "combat/hud/life.h"
#include "combat/hud/cmd_plr.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/enemy/dummy.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void initializeCombatants();
  void initializePlayer();
  void initializeCompanion();

  void update() override;
  void eventProcessing();
  void eventHandling(std::unique_ptr<CombatEvent> &event);

  void dmgNumberHandling(Combatant *target, DamageType damage_type,
                         float damage_taken);
  void deleteEntity(int entity_id);
  void endCombatProcedure();
  void updatePartyAttr(PartyMember *member, Character *data);

  void draw() override;

  void drawDebugInfo();
  void drawPartyStats(PartyMember *member, Vector2 position, Font *font, 
                      int text_size);
  void drawDebugCombo(Font *font, int text_size);

  static SpriteAtlas cmd_atlas;
private:
  CombatStage stage;
  Texture debug_overlay;

  CombatCamera camera;
  CombatHandler evt_handler;
  CombatantHandler cbt_handler;

  bool game_over = false;

  Mary *player;
  LifeHud plr_hud = LifeHud({34, 215});
  PlayerCmdHud plr_cmd_hud = PlayerCmdHud({350, 178});

  PartyMember *companion;
  LifeHud com_hud = LifeHud({154, 215});

  // Remove this later!
  Dummy *dummy = NULL;

  std::vector<std::unique_ptr<Entity>> entities;
  Session *session;
};
