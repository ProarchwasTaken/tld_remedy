#pragma once
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"


class CampMenuScene : public Scene {
public:
  CampMenuScene(Session *session);
  ~CampMenuScene();

  void update() override;
  void offsetBars();

  void draw() override;
  void drawTopBar();
  void drawHeader(Vector2 position);
  void drawTopInfo(Vector2 position);

  void drawBottomBar();
  void drawBottomInfo(Vector2 position);

  static SpriteAtlas atlas;
private:
  Session *session;
  MenuKeybinds *keybinds;

  Texture main_bar;

  float bar_offset = 0;
  float offset_speed = 110;

  const Vector2 top_position = {0, 0};
  const Vector2 bottom_position = {0, 200};
};
