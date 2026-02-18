#pragma once
#include <array>
#include <cstddef>
#include <unordered_set>
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "combat/hud/blackbars.h"


enum class RestMenuOptions {
  DIAGNOSE,
  CRAFTING,
  TALK,
  SAVE,
  LEAVE
};


class RestMenuScene : public Scene {
public:
  RestMenuScene(Session *session);
  ~RestMenuScene();

  void loadBackground(std::string location);
  void setupCharacters();

  void update() override;
  void openingLogic();
  void closingLogic();
  void normalLogic();
  void openingPanel();
  void closingPanel();

  void optionNavigation();
  void selectOption();
  void panelTermination();

  void flickering();

  void draw() override;
  void drawBackground();
  void drawOptions();
  void baseOptionLerp(Vector2 &position, Color &color);
  void selectedLogic(float clock, Vector2 &position, Color &color);
  void unselectedLogic(Vector2 &position, Color &color);
  const char *getOptionText(RestMenuOptions option);
private:
  Session *session;
  MenuKeybinds *keybinds;

  Camera2D camera;
  BlackBars black_bars = BlackBars(120, true);

  Texture background;
  Color bg_color;
  Color char_color;
 
  Rectangle *plr_sprite;
  Vector2 plr_position;

  Rectangle *com_sprite;
  Vector2 com_position;

  enum {
    OPENING,
    CLOSING,
    READY,
    OPENING_PANEL,
    CLOSING_PANEL
  } state = OPENING;

  std::array<RestMenuOptions, 5> options = {
    RestMenuOptions::DIAGNOSE,
    RestMenuOptions::CRAFTING,
    RestMenuOptions::TALK,
    RestMenuOptions::SAVE,
    RestMenuOptions::LEAVE
  };

  std::array<RestMenuOptions, 5>::iterator selected = options.begin();
  std::array<RestMenuOptions, 5>::iterator prev_selected = NULL;
  std::unordered_set<RestMenuOptions> disallowed = {
    RestMenuOptions::TALK
  };

  float state_clock = 0.0;
  float state_time = 2.0;

  float tick_clock = 0.0;
  float tick_time = 1.0 / 60.0;

  float opt_clock = 0.0;
  float opt_time = 0.2;

  float panel_clock = 0.0;
  float panel_time = 0.4;

  SpriteAtlas atlas = SpriteAtlas("menu", "rest_menu");
  SoundAtlas *sfx;
};
