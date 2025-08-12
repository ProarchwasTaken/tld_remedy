#pragma once
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"


class ConfigPanel : public Panel {
public:
  ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas, 
              MenuKeybinds *keybinds);
  ~ConfigPanel();

  void update() override;
  void frameTransition();

  void draw() override;
private:
  enum {READY, OPENING, CLOSING} state = OPENING;
  float clock = 0.0;
  float transition_time = 0.25;

  Texture frame;
  Vector2 position;
  float frame_height = 161;

  SpriteAtlas *atlas;
  MenuKeybinds *keybinds;
};
