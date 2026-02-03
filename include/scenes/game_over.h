#pragma once
#include "base/scene.h"


class GameOverScene : public Scene {
public:
  GameOverScene();
  void startDialog();
  void startClosingDialog();

  void update() override;
  void startup();

  void panelLogic();
  bool responseHandling();

  void draw() override;
private:
  bool ready = false;
  bool exiting = false;

  float start_time = 4.0;
  float start_clock = 0.0;
};
