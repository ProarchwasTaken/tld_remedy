#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class ConfirmOption {
  YES,
  NO
};


class ConfirmPanel : public Panel {
public:
  ConfirmPanel(SpriteAtlas *menu_atlas, MenuKeybinds *menu_keybinds,
               std::string message, 
               std::string txt_yes = "YES", 
               std::string txt_no = "NO");
  ~ConfirmPanel();

  void update() override;
  void heightLerp();
  void draw() override;

  std::array<ConfirmOption, 2>::iterator selected;
private:
  Texture frame;
  Vector2 position = {109, 80};

  float frame_height = 80;
  float blink_clock = 0;

  std::array<ConfirmOption, 2> options {
    ConfirmOption::YES,
    ConfirmOption::NO
  };

  std::string message;
  std::string txt_yes;
  std::string txt_no;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
