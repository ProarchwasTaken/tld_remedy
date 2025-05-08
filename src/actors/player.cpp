#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "utils/input.h"
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

  position.x += (movement_speed * Game::deltaTime()) * x_direction;
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

  position.y += (movement_speed * Game::deltaTime()) * y_direction;
}

void PlayerActor::draw() {

}

