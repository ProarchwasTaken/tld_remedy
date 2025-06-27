#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "data/keybinds.h"
#include "data/actor_event.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "utils/collision.h"
#include "utils/animation.h"
#include "field/system/actor_handler.h"
#include "field/entities/pickup.h"
#include "field/actors/player.h"

using std::unique_ptr;

bool PlayerActor::controllable = true;
FieldKeybinds PlayerActor::key_bind;
SpriteAtlas PlayerActor::atlas("actors", "mary_actor");


PlayerActor::PlayerActor(Vector2 position, enum Direction direction):
Actor("Mary", ActorType::PLAYER, position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
  atlas.use();

  sprite = getIdleSprite();
}

PlayerActor::~PlayerActor() {
  if (pickup_event != nullptr) {
    pickup_event.reset();
    PLOGD << "Cleared held event: PickupInRange";
  }

  atlas.release();
}

void PlayerActor::behavior() {
  processEvents();

  if (controllable) {
    bool gamepad = IsGamepadAvailable(0);

    movementInput(gamepad);
    interactInput(gamepad);
  }
}

void PlayerActor::processEvents() {
  EventPool<ActorEvent> *event_pool = ActorHandler::get();
  int count = event_pool->size();

  for (int x = 0; x < count; x++) {
    unique_ptr<ActorEvent> &event = event_pool->at(x);

    if (event == nullptr) {
      continue;
    }

    bool holding_event = pickup_event != nullptr;
    ActorEVT type = event->event_type;
    if (!holding_event && type == ActorEVT::PICKUP_IN) {
      int id = event->sender->entity_id;

      PLOGD << "Player in range of Pickup Entity [ID: " << id << "]";
      pickup_event.swap(event);
    } 
    else if (holding_event && type == ActorEVT::PICKUP_OUT) {
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

  moving = isMoving();
}

bool PlayerActor::isMoving() {
  if (moving_x != 0 || moving_y != 0) {
    return true;
  }
  else {
    return false;
  }
}

void PlayerActor::interactInput(bool gamepad) {
  bool interact = Input::pressed(key_bind.interact, gamepad);

  if (interact && pickup_event != nullptr) {
    Pickup *pickup = static_cast<Pickup*>(pickup_event->sender);

    PLOGI << "Interacting with Pickup [ID: " << pickup->entity_id << "]";
    pickup->interact();

    pickup_event.reset();
    return;
  }
}

void PlayerActor::update() {
  Vector2 old_position = position;

  moveX();
  moveY();

  has_moved = !Vector2Equals(old_position, position);
  if (has_moved) {
    move_clock += Game::time() / move_interval;
    moveAnimation();
    rectExCorrection(bounding_box, collis_box);
  }
  else {
    sprite = getIdleSprite();
  }

  if (move_clock >= 1.0) {
    ActorHandler::queue<ActorEvent>(this, ActorEVT::PLR_MOVING);
    move_clock = 0.0;
  }
}

void PlayerActor::moveX() {
  if (moving_x == 0) {
    return;
  }
  direction = static_cast<Direction>(moving_x);

  float speed = default_speed;
  if (moving_y != 0) {
    speed = Normalize(default_speed, 0, speed_root);
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
    speed = Normalize(default_speed, 0, 1.45);
  }

  float magnitude = speed * Game::deltaTime();

  float collision_y;
  if (Collision::checkY(this, magnitude, moving_y, collision_y)) {
    Collision::snapY(this, collision_y, moving_y);
    return;
  }

  position.y += magnitude * moving_y;
}

Rectangle *PlayerActor::getIdleSprite() {
  animation = NULL;

  switch (direction) {
    case DOWN: {
      return &atlas.sprites[1];
    }
    case RIGHT: {
      return &atlas.sprites[4];
    }
    case UP: {
      return &atlas.sprites[7];
    }
    case LEFT: {
      return &atlas.sprites[10];
    }
  }
}

void PlayerActor::moveAnimation() {
  Animation *next_anim;

  switch (direction) {  
    case DOWN: {
      next_anim = &anim_down;
      break;
    }
    case RIGHT: {
      next_anim = &anim_right;
      break;
    }
    case UP: {
      next_anim = &anim_up;
      break;
    }
    case LEFT: {
      next_anim = &anim_left;
      break;
    }
  }

  SpriteAnimation::play(animation, next_anim, true);
  sprite = &atlas.sprites[*animation->current];
}


void PlayerActor::draw() {
  assert(sprite != NULL);
  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);
}

