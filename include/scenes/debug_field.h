#pragma once
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "system/field_map.h"


class DebugField : public Scene {
public:
  DebugField();
  ~DebugField();

  void update() override;
  void draw() override;
private:
  FieldMap field;

  Camera2D camera;
  Entity *camera_target;

  std::vector<std::unique_ptr<Entity>> entities;
};
