#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class TechsPanel : public Panel {
public:
  TechsPanel(Session *session, std::string *description);
  ~TechsPanel();

  void update() override;
  void heightLerp();
  void optionNavigation();

  void draw() override;

  void drawOptions();
  void drawCursor(Vector2 position);
private:
  SpriteAtlas *camp_atlas;
  SpriteAtlas *menu_atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  Texture frame;
  constexpr static Vector2 frame_position = {192, 44};
  float frame_height = 0;
  float percentage = 0.0;

  constexpr static Vector2 option_position = {41, 58};
  std::array<Character*, 2> options;
  std::array<Character*, 2>::iterator selected;
  float blink_clock = 0.0;
};
