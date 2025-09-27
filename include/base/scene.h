#pragma once
#include <memory>
#include "enums.h"
#include "base/panel.h"


class Scene {
public:
  virtual ~Scene() = default;

  virtual void update() = 0;
  virtual void draw() = 0;

  SceneID scene_id;
protected:
  bool panel_mode = false;
  std::unique_ptr<Panel> panel;
};
