#pragma once
#include <memory>
#include <raylib.h>
#include "data/keybinds.h"
#include "enums.h"
#include "base/actor.h"
#include "data/actor_event.h"


class PlayerActor : public Actor {
public:
  PlayerActor(Vector2 position, enum Direction direction);
  ~PlayerActor();

  void behavior() override;
  void processEvents();
  void dropPickupEvent(std::unique_ptr<ActorEvent> &out_range);
  void movementInput(bool gamepad);
  bool isMoving();
  void interactInput(bool gamepad);

  void update() override;
  void moveX();
  void moveY();

  void draw() override;

  static void setControllable(bool value);
  inline static FieldKeybinds key_bind;

  bool moving = false;
private:
  static bool controllable;
  std::unique_ptr<ActorEvent> pickup_event;

  const float default_speed = 1.1;
  int moving_x = 0;
  int moving_y = 0;

  float move_clock = 0.0;
  float move_interval = 0.35;
};
