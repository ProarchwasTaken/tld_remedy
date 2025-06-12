#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/session.h"


class CombatScene : public Scene {
public:
  CombatScene(Session *session);
  ~CombatScene();

  void update() override;
  void draw() override;
private:
  Player *player;
  Companion *companion;

  std::vector<std::unique_ptr<Entity>> entities;
};
