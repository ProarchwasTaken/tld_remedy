#pragma once
#include <memory>
#include <raylib.h>
#include "data/keybinds.h"
#include "enums.h"
#include "base/actor.h"
#include "data/actor_event.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"


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
  Rectangle *getIdleSprite();
  void moveAnimation();

  void draw() override;

  static void setControllable(bool value);
  static FieldKeybinds key_bind;
  static SpriteAtlas atlas;

  bool moving = false;
  bool has_moved = false;
private:
  static bool controllable;

  const float default_speed = 56;
  int moving_x = 0;
  int moving_y = 0;

  float move_clock = 1.0;
  float move_interval = 0.30;

  Animation anim_down = {{0, 1, 2, 1}, 0.25};
  Animation anim_right = {{3, 4, 5, 4}, 0.25};
  Animation anim_up = {{6, 7, 8, 7}, 0.25};
  Animation anim_left = {{9, 10, 11, 10}, 0.25};

  std::unique_ptr<ActorEvent> pickup_event;
};
