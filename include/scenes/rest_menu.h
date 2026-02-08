#pragma once
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"
#include "system/sprite_atlas.h"


class RestMenuScene : public Scene {
public:
  RestMenuScene(Session *session);
  ~RestMenuScene();

  void loadBackground(std::string location);
  void setupCharacters();

  void update() override;
  void draw() override;
private:
  Session *session;
  Camera2D camera;

  Texture background;
  Color bg_color;
  Color char_color;
 
  Rectangle *plr_sprite;
  Vector2 plr_position;

  Rectangle *com_sprite;
  Vector2 com_position;

  SpriteAtlas atlas = SpriteAtlas("menu", "rest_menu");
};
