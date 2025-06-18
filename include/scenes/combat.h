#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/session.h"
#include "combat/system/stage.h"
#include "combat/system/camera.h"
#include "combat/combatants/player.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void update() override;
  void draw() override;
  void drawDebugInfo();
private:
  CombatCamera camera;

  CombatStage stage;
  Texture debug_overlay;

  PlayerCombatant *player;
  std::vector<std::unique_ptr<Entity>> entities;
};
