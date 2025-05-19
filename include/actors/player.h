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

  static void setControllable(bool value);
  inline static FieldKeybinds key_bind;

  bool moving = false;
private:
  static bool controllable;

  const float default_speed = 1.1;
  int moving_x = 0;
  int moving_y = 0;
};
