#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"


class PlayerActor : public Actor {
public:
  PlayerActor(Vector2 position, enum Direction direction);

  void behavior() override;
  void update() override;
  void draw() override;
};
