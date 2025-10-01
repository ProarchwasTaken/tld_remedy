#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class StatusPanel : public Panel {
public:
  StatusPanel(Session *session, std::string *description);
  ~StatusPanel();

  void update() override;
  void heightLerp();

  void optionNavigation();
  void draw() override;
  void drawOptions();
  void drawCursor(Vector2 position);
private:
  std::string *description;
  SpriteAtlas *camp_atlas;
  SpriteAtlas *menu_atlas;
  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {257, 48};
  constexpr static Vector2 option_position = {41, 58};

  std::array<Character*, 2> options;
  std::array<Character*, 2>::iterator selected;
  float blink_clock = 0.0;
};
