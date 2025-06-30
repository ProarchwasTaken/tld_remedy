#pragma once
#include <raylib.h>
#include "base/enemy.h"
#include "enums.h"
#include "system/sprite_atlas.h"


class Dummy : public Enemy {
public:
  Dummy(Vector2 position, Direction direction);
  ~Dummy();

  void behavior() override {};
  void attack();

  void update() override;
  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  Rectangle *sprite;
};
