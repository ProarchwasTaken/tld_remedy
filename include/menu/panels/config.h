#pragma once
#include <raylib.h>
#include "base/panel.h"
#include "system/sprite_atlas.h"


class ConfigPanel : public Panel {
public:
  ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas);
  ~ConfigPanel();

  void update() override;
  void draw() override;
private:
  Texture frame;
  Vector2 position;

  SpriteAtlas *atlas;
};
