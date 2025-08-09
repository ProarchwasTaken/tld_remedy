#pragma once
#include "base/scene.h"


class TitleScene : public Scene {
public:
  TitleScene();
  ~TitleScene();

  void update() override;
  void draw() override;
};
