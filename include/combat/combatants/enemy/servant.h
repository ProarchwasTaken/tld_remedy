#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/enemy.h"
#include "system/sprite_atlas.h"


class Servant : public Enemy {
public:
  Servant(Vector2 position, Direction direction);
  ~Servant();

  void update() override;

  Rectangle *getStunSprite();
  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
};
