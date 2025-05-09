#include <raylib.h>
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
}

void PlayerActor::movementInput(bool gamepad) {
  if (Input::pressed(key_bind.move_right, gamepad) && !moving_right) {
    moving_right = true;
  } 
  if (Input::pressed(key_bind.move_left, gamepad) && !moving_left) {
    moving_left = true;
  }
  if (Input::pressed(key_bind.move_down, gamepad) && !moving_down) {
    moving_down = true;
  }
  if (Input::pressed(key_bind.move_up, gamepad) && !moving_up) {
    moving_up = true;
  }

  if (Input::released(key_bind.move_right, gamepad) && moving_right) {
    moving_right = false;
  }
  if (Input::released(key_bind.move_left, gamepad) && moving_left) {
    moving_left = false;
  }
  if (Input::released(key_bind.move_down, gamepad) && moving_down) {
    moving_down = false;
  }
  if (Input::released(key_bind.move_up, gamepad) && moving_up) {
    moving_up = false;
  }
}

void PlayerActor::update() {
  moveX();
  moveY();
  rectExCorrection(bounding_box, collis_box);
}

void PlayerActor::moveX() {
  int x_direction;
  if (moving_left == moving_right) {
    return;
  }
  else if (moving_right) {
    direction = Direction::RIGHT;
    x_direction = 1;
  }
  else {
    direction = Direction::LEFT;
    x_direction = -1;
  }

  float magnitude = movement_speed * Game::deltaTime();
  float collision_x;

  if (!Collision::checkX(this, magnitude, x_direction, collision_x)) {
    position.x += magnitude * x_direction;
    return;
  }

  float half_scale_x = collis_box.scale.x / 2;
  float collis_x;
  if (x_direction == 1) {
    collis_x = collis_box.position.x + collis_box.scale.x; 
  }
  else {
    collis_x = collis_box.position.x;
  }

  float difference = position.x - collis_x;
  position.x = collision_x + difference;
}

void PlayerActor::moveY() {
  int y_direction;
  if (moving_down == moving_up) {
    return;
  }
  else if (moving_down) {
    direction = Direction::DOWN;
    y_direction = 1;
  }
  else {
    direction = Direction::UP;
    y_direction = -1;
  }

  float magnitude = movement_speed * Game::deltaTime();
  float collision_y;
  if (!Collision::checkY(this, magnitude, y_direction, collision_y)) {
    position.y += magnitude * y_direction;
    return;
  }

  float half_scale_y = collis_box.scale.y / 2;
  float collis_y;

  if (y_direction == 1) {
    collis_y = collis_box.position.y + collis_box.scale.y;
  }
  else {
    collis_y = collis_box.position.y;
  }

  float difference = position.y - collis_y;
  position.y = collision_y + difference;
}

void PlayerActor::draw() {

}

