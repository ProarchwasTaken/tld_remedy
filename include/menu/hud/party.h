#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"
#include "data/session.h"


class PartyHud {
public:
  PartyHud(Vector2 position);
  ~PartyHud();

  void assign(Character *user);

  void draw();
  void drawName();
  void drawLife();
  void drawMorale();
  void drawStatusIcons();

private:
  Character *user;
  SpriteAtlas *atlas;
  static SpriteAtlas icon_atlas;

  Vector2 main_position;
};
