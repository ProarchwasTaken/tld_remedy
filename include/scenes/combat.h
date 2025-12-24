#pragma once
#include <raylib.h>
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "base/party_member.h"
#include "base/enemy.h"
#include "data/session.h"
#include "data/combat_event.h"
#include "data/combatant_event.h"
#include "data/enemy_troops.h"
#include "system/sprite_atlas.h"
#include "combat/system/stage.h"
#include "combat/system/camera.h"
#include "combat/system/evt_handler.h"
#include "combat/system/cbt_handler.h"
#include "combat/hud/blackbars.h"
#include "combat/hud/life.h"
#include "combat/hud/enemy.h"
#include "combat/hud/combo.h"
#include "combat/hud/toasts.h"
#include "combat/hud/cmd_plr.h"
#include "combat/hud/cmd_assist.h"
#include "combat/hud/cmd_item.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/enemy/dummy.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void initializeCombatants();
  void initializePlayer();
  void initializeCompanion();

  void initializeTroop(EnemyTroop *troop);
  std::unique_ptr<Enemy> createEnemy(EnemyData &data);

  void update() override;
  void combatantBehavior();
  void eventEvaluation(std::unique_ptr<CombatantEvent> &event);
  void eventProcessing();
  void updateHud();
  void eventHandling(std::unique_ptr<CombatEvent> &event);

  void dmgNumberHandling(Combatant *target, DamageType damage_type,
                         float damage_taken);
  void deleteEntity(int entity_id);
  void endCombatProcedure();
  void updatePartyAttr(PartyMember *member, Character *data);

  void draw() override;
  void drawHud();

  #ifndef NDEBUG
  void debugKeybinds();
  void drawDebugInfo();
  void drawPartyStats(PartyMember *member, Vector2 position, Font *font, 
                      int text_size);
  void drawDebugCombo(Font *font, int text_size); 
  #endif // !NDEBUG

  static SpriteAtlas cmd_atlas;
private:
  CombatStage stage;
  Texture debug_overlay;

  CombatCamera camera;
  BlackBars black_bars;
  CombatHandler evt_handler;
  CombatantHandler cbt_handler;

  bool game_over = false;

  Mary *player = NULL;
  std::unique_ptr<LifeHud> plr_hud;
  std::unique_ptr<PlayerCmdHud> plr_cmd_hud;

  PartyMember *companion = NULL;
  std::unique_ptr<LifeHud> com_hud;
  std::unique_ptr<AssistCmdHud> assist_hud;

  std::unique_ptr<EnemyHud> enemy_hud;
  std::unique_ptr<ItemCmdHud> item_hud;
  std::unique_ptr<ComboHud> combo_hud;
  std::unique_ptr<CombatToasts> toasts;

  // Remove this later!
  Dummy *dummy = NULL;

  std::vector<std::unique_ptr<Entity>> entities;
  Session *session;
};
