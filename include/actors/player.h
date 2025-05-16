#pragma once
#include <raylib.h>
#include "data/keybinds.h"
#include "enums.h"
#include "base/actor.h"


class PlayerActor : public Actor {
public:
  PlayerActor(Vector2 position, enum Direction direction);

  void behavior() override;
  void movementInput(bool gamepad);
  bool isMoving();

  void update() override;
  void moveX();
  void moveY();

  void draw() override;

  inline static FieldKeybinds key_bind;

  bool moving = false;
private:
  bool moving_right = false;
  bool moving_left = false;
  bool moving_down = false;
  bool moving_up = false;
  float movement_speed = 1.0;
};
