#pragma once
#include <raylib.h>
#include "base/enemy.h"
#include "enums.h"


class Dummy : public Enemy {
public:
  Dummy(Vector2 position, Direction direction);

  void behavior() override {};
  void attack();

  void update() override;
  void draw() override;
  void drawDebug() override;
};
