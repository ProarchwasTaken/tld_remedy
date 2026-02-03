#pragma once
#include <string>
#include "base/scene.h"


class GameOverScene : public Scene {
public:
  GameOverScene(std::string reason);
  void startDialog();
  void startClosingDialog();

  void update() override;
  void startup();

  void panelLogic();
  bool responseHandling();

  void draw() override;
  void drawGameOverText();
private:
  bool ready = false;
  bool exiting = false;

  float start_time = 6.0;
  float start_clock = 0.0;

  std::string reason;
};
