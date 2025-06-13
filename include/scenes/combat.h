#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/session.h"
#include "combat/combatants/player.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void update() override;
  void draw() override;
  void drawDebugInfo();
private:
  Camera2D camera;
  PlayerCombatant *player;
  std::vector<std::unique_ptr<Entity>> entities;
};
