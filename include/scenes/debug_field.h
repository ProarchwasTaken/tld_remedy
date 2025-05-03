#pragma once
#include "base/scene.h"


class DebugField : public Scene {
public:
  DebugField();

  void update() override;
  void draw() override;
};
