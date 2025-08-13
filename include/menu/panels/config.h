#pragma once
#include <array>
#include <utility>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class ConfigOption {
  VOL_MASTER,
  VOL_SOUND,
  VOL_MUSIC,
  FULLSCREEN,
  FRAMERATE,
  CONTROLS,
  APPLY,
  BACK
};


class ConfigPanel : public Panel {
public:
  ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas, 
              MenuKeybinds *keybinds);
  ~ConfigPanel();
  void setupOptions();

  void update() override;
  void frameTransition();

  void optionNavigation();
  void nextOption();
  void prevOption();

  void draw() override;
  void drawOptions();
  void drawCursor(Vector2 position);
  std::string getOptionName(ConfigOption id);
private:
  enum {READY, OPENING, CLOSING} state = OPENING;
  float clock = 0.0;
  float blink_clock = 0;
  float transition_time = 0.25;

  Texture frame;
  Vector2 position;
  float frame_height = 161;

  std::array<std::pair<ConfigOption, float>, 8> options;
  std::array<std::pair<ConfigOption, float>, 8>::iterator selected;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
  bool on_linux;
};
