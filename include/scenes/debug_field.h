#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"


class DebugField : public Scene {
public:
  DebugField();
  ~DebugField();

  void update() override;
  void draw() override;
private:
  std::vector<std::unique_ptr<Entity>> entities;
};
