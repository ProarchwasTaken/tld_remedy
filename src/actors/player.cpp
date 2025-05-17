#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "utils/input.h"
#include "utils/collision.h"
#include "actors/player.h"


PlayerActor::PlayerActor(Vector2 position, enum Direction direction):
Actor("Mary", ActorType::PLAYER, position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
}

void PlayerActor::behavior() {
  bool gamepad = IsGamepadAvailable(0);

  movementInput(gamepad);
  moving = isMoving();
}

void PlayerActor::movementInput(bool gamepad) {
  bool right = Input::down(key_bind.move_right, gamepad);
  bool left = Input::down(key_bind.move_left, gamepad);

  moving_x = right - left;

  bool down = Input::down(key_bind.move_down, gamepad);
  bool up = Input::down(key_bind.move_up, gamepad);

  moving_y = down - up;
}

bool PlayerActor::isMoving() {
  if (moving_x != 0 || moving_y != 0) {
    return true;
  }
  else {
    return false;
  }
}

void PlayerActor::update() {
  moveX();
  moveY();
  rectExCorrection(bounding_box, collis_box);
}

void PlayerActor::moveX() {
  if (moving_x == 0) {
    return;
  }
  direction = static_cast<Direction>(moving_x);

  float speed = default_speed;
  if (moving_y != 0) {
    speed = Normalize(default_speed, 0, 1.4);
  }

  float magnitude = speed * Game::deltaTime();
  float collision_x;
  if (Collision::checkX(this, magnitude, moving_x, collision_x)) {
    float half_scale_x = collis_box.scale.x / 2;
    float collis_x = collis_box.position.x + half_scale_x;
    collis_x += half_scale_x * moving_x;

    float difference = position.x - collis_x;
    position.x = collision_x + difference;
    return;
  }

  position.x += magnitude * moving_x;
}

void PlayerActor::moveY() {
  if (moving_y == 0) {
    return;
  }
  direction = static_cast<Direction>(moving_y * 2);

  float speed = default_speed;
  if (moving_x != 0) {
    speed = Normalize(default_speed, 0, 1.4);
  }

  float magnitude = speed * Game::deltaTime();

  float collision_y;
  if (Collision::checkY(this, magnitude, moving_y, collision_y)) {
    float half_scale_y = collis_box.scale.y / 2;
    float collis_y = collis_box.position.y + half_scale_y;
    collis_y += half_scale_y * moving_y;

    float difference = position.y - collis_y;
    position.y = collision_y + difference;
    return;
  }

  position.y += magnitude * moving_y;
}

void PlayerActor::draw() {

}

