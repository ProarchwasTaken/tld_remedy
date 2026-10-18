#pragma once
#include "enums.h"
#include "base/scene.h"
#include "data/session.h"
#include "system/sprite_atlas.h"

enum class SetupPhase {
  SELECT_GAMEMODE,
  SELECT_SUBWEAPON,
  SELECT_COMPANION,
  CONFIRMATION
};


/* This scene is where the player gets to decide the starting 
 * conditions of their new session. Mainly accessible through from the 
 * TitleScene by selecting "NEW GAME"*/
class SetupScene : public Scene {
public:
  SetupScene();
  ~SetupScene();

  void initPanel();

  void initPlayer(SubWeaponID weapon_id);
  void initCompanion(CompanionID companion_id);

  void update() override;
  void panelTermination();

  void draw() override;

  static SpriteAtlas atlas;
private:
  GameMode gamemode = GameMode::INDIFFERENCE;
  Player player;
  Companion companion;

  SetupPhase phase = SetupPhase::SELECT_SUBWEAPON;
};
