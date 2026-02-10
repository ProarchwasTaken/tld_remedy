#pragma once
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "combat/hud/blackbars.h"


class RestMenuScene : public Scene {
public:
  RestMenuScene(Session *session);
  ~RestMenuScene();

  void loadBackground(std::string location);
  void setupCharacters();

  void update() override;
  void openingLogic();
  void flickeringLogic();
  void draw() override;
private:
  Session *session;
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
    READY
  } state = OPENING;

  float state_clock = 0.0;
  float state_time = 2.0;

  float tick_clock = 0.0;
  float tick_time = 1.0 / 60.0;

  SpriteAtlas atlas = SpriteAtlas("menu", "rest_menu");
};
