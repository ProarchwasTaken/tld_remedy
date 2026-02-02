#pragma once
#include "base/scene.h"


class GameOverScene : public Scene {
public:
  GameOverScene();
  void startDialog();

  void update() override;
  void panelLogic();
  void draw() override;
};
