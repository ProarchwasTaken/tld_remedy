#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "data/actor_event.h"
#include "utils/input.h"
#include "utils/collision.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"

using std::unique_ptr;
bool PlayerActor::controllable = true;

PlayerActor::PlayerActor(Vector2 position, enum Direction direction):
Actor("Mary", ActorType::PLAYER, position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
}

PlayerActor::~PlayerActor() {
  if (pickup_event != nullptr) {
    pickup_event.reset();
  }
}

void PlayerActor::behavior() {
  processEvents();

  if (!controllable) {
    return;
  }

  bool gamepad = IsGamepadAvailable(0);

  movementInput(gamepad);
  moving = isMoving();
}

void PlayerActor::processEvents() {
  for (unique_ptr<ActorEvent> &event : *ActorHandler::get()) {
    if (event == nullptr) {
      continue;
    }

    bool holding_event = pickup_event != nullptr;
    if (!holding_event && event->event_type == PICKUP_IN_RANGE) {
      int id = event->sender->entity_id;

      PLOGD << "Player in range of Pickup Entity [ID: " << id << "]";
      pickup_event.swap(event);
    } 
    else if (holding_event && event->event_type == PICKUP_OUT_RANGE) {
      dropPickupEvent(event);
    }
  }
}

void PlayerActor::dropPickupEvent(unique_ptr<ActorEvent> &out_range) {
  int first_id = pickup_event->sender->entity_id;
  int second_id = out_range->sender->entity_id;

  bool from_same_entity = first_id == second_id;

  if (from_same_entity) {
    PLOGD << "Player has exited range of Pickup Entity [ID: " 
      << second_id << "]";
    pickup_event.reset();
  }
}

void PlayerActor::setControllable(bool value) {
  controllable = value;

  if (controllable) {
    PLOGI << "Control has been given to the player.";
  }
  else {
    PLOGI << "Control has been revoked from the player.";
  }
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
    Collision::snapX(this, collision_x, moving_x);
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
    Collision::snapY(this, collision_y, moving_y);
    return;
  }

  position.y += magnitude * moving_y;
}

void PlayerActor::draw() {

}

