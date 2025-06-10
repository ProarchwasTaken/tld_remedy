#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "field/system/actor_handler.h"
#include "field/actors/companion.h"

using std::unique_ptr;
SpriteAtlas CompanionActor::atlas("actors", "erwin_actor");


CompanionActor::CompanionActor(Vector2 position, 
                               enum Direction direction):
Actor("Companion", ActorType::COMPANION, position, direction)
{
  bounding_box.scale = {32, 34};
  bounding_box.offset = {-16, -30};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
  atlas.use();
}

CompanionActor::~CompanionActor() {
  atlas.release();
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
      PlayerActor *sender = static_cast<PlayerActor*>(event->sender);
      Vector2 position = sender->position;
      Direction direction = sender->direction;

      move_points.push_back({position, direction});
      if (plr == NULL) plr = sender;
    }
  }
}

void CompanionActor::update() {
  moving = plr != NULL && plr->has_moved;
  if (moving && !move_points.empty()) {
    pathfind();
    rectExCorrection(bounding_box, collis_box);
  }
}

void CompanionActor::pathfind() {
  auto *first_point = &move_points.front();
  
  Vector2 target = first_point->first;
  direction = first_point->second;

  float speed = movement_speed * Game::deltaTime();
  position = Vector2MoveTowards(position, target, speed);

  if (Vector2Equals(position, target)) {
    move_points.pop_front();
  }
}

void CompanionActor::draw() {
  Rectangle *sprite;
  if (!moving) {
    sprite = getIdleSprite();
  }
  else {
    sprite = getWalkSprite();
  }


  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);
}

Rectangle *CompanionActor::getIdleSprite() {
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

Rectangle *CompanionActor::getWalkSprite() {
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

  if (animation != next_anim) {
    animation = next_anim;
    animation->current = animation->frames.begin();
    animation->frame_clock = 0.0;
  }

  SpriteAnimation::play(*animation, true);
  return &atlas.sprites[*animation->current];
}

void CompanionActor::drawDebug() {
  Actor::drawDebug();

  if (move_points.empty()) {
    return;
  }

  auto *first_point = &move_points.front();
  DrawLineV(position, first_point->first, YELLOW);

  for (auto point : move_points) {
    DrawCircleV(point.first, 2, YELLOW);
  }
}

