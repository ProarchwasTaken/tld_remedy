#pragma once
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/personal.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class RemapOption {
  INPUT_RIGHT,
  INPUT_LEFT,
  INPUT_DOWN,
  INPUT_UP,
  INPUT_A,
  INPUT_B,
  INPUT_X,
  INPUT_Y,
  INPUT_L,
  INPUT_R,
  INPUT_START,
  INPUT_SELECT
};


class RemapPanel : public Panel {
public:
  RemapPanel(SpriteAtlas *menu_atlas, Settings *settings);
  ~RemapPanel();

  void update() override;
  void heightLerp();
  void optionNavigation();

  void draw() override;
private:
  Texture frame;
  const Vector2 main_position = {60, 4};

  float frame_height = 0;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
  Settings *settings;
};
