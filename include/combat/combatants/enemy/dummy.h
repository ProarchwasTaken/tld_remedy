#pragma once
#include <raylib.h>
#include "base/enemy.h"
#include "enums.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"


class Dummy : public Enemy {
public:
  Dummy(Vector2 position, Direction direction);
  ~Dummy();

  void behavior() override {};
  void attack();

  void update() override;
  void draw() override;
  void drawDebug() override;

  AttackAnimSet atk_set = {
    {{2, 3}, 0.05},
    {{3, 2}, 0.05},
    4
  };
  static SpriteAtlas atlas;
};
