#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "data/actor_event.h"
#include "field/system/actor_handler.h"
#include "field/actors/companion.h"

using std::unique_ptr;


CompanionActor::CompanionActor(Vector2 position, 
                               enum Direction direction):
Actor("Companion", ActorType::COMPANION, position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
}

void CompanionActor::behavior() {
  processEvents();
}

void CompanionActor::processEvents() {
  EventPool<ActorEvent> *event_pool = ActorHandler::get();
  int count = event_pool->size();

  for (int x = 0; x < count; x++) {
    unique_ptr<ActorEvent> &event = event_pool->at(x);

    if (event == nullptr) {
      continue;
    }

    if (event->event_type == PLR_MOVING) {
      Vector2 position = event->sender->position;
      move_points.push_back(position);
    }
  }
}

void CompanionActor::update() {
  if (!move_points.empty()) {
    pathfind();
    rectExCorrection(bounding_box, collis_box);
  }
}

void CompanionActor::pathfind() {
  Vector2 first_point = move_points.front();

  Vector2 difference = Vector2Subtract(first_point, position);
  if (difference.x >= 0) {
    direction = RIGHT;
  }
  else {
    direction = LEFT;
  }

  if (difference.y >= 0) {
    direction = DOWN;
  }
  else {
    direction = UP;
  }

  float speed = movement_speed * Game::deltaTime();
  position = Vector2MoveTowards(position, first_point, speed);

  if (Vector2Equals(position, first_point)) {
    move_points.pop_front();
  }
}

void CompanionActor::draw() {

}

void CompanionActor::drawDebug() {
  Actor::drawDebug();

  if (move_points.empty()) {
    return;
  }

  Vector2 first_point = move_points.front();
  DrawLineV(position, first_point, YELLOW);

  for (Vector2 point : move_points) {
    DrawCircleV(point, 2, YELLOW);
  }
}

