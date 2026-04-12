#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "base/actor.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "field/actors/companion.h"

using std::unique_ptr, std::string;
SpriteAtlas CompanionActor::atlas("actors", "");


CompanionActor::CompanionActor(CompanionID id, Vector2 position, 
                               enum Direction direction):
Actor("Companion", ActorType::COMPANION, position, direction)
{
  default_speed = 56.5;
  movement_speed = default_speed;

  setupAtlas(id);
  rectExCorrection(bounding_box, collis_box);
  
  atlas.use();
  sprite = getIdleSprite();
}

CompanionActor::~CompanionActor() {
  atlas.release();
}

void CompanionActor::setupAtlas(CompanionID id) {
  assert(atlas.users() == 0);
  string sprite_group;

  switch (id) {
    case CompanionID::ERWIN: {
      sprite_group = "erwin_actor";

      bounding_box.scale = {32, 34};
      bounding_box.offset = {-16, -30};
      collis_box.scale = {8, 16};
      collis_box.offset = {-4, -12};
      break;
    }
  }

  atlas = SpriteAtlas("actors", sprite_group);
}

void CompanionActor::evaluateEvent(unique_ptr<ActorEvent> &event) {
  ActorEVT type = event->event_type;
  if (follow_player && type == ActorEVT::PLR_MOVING) {
    PlayerActor *sender = static_cast<PlayerActor*>(event->sender);
    Vector2 position = sender->position;
    Direction direction = sender->direction;

    move_points.push_back({position, direction});
    if (plr == NULL) {
      plr = sender;
    } 
  }
}

void CompanionActor::update() {
  moving = shouldBeMoving();
  if (!moving) {
    sprite = getIdleSprite();
    return;
  }

  moveAnimation();

  if (!move_points.empty()) {
    pathfind();
    rectExCorrection(bounding_box, collis_box);
  }
}

bool CompanionActor::shouldBeMoving() {
  if (follow_player && plr != NULL && plr->has_moved) {
    return true;
  }
  else if (!follow_player && !move_points.empty()) {
    return true;
  }
  else {
    return false;
  }
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

void CompanionActor::moveAnimation() {
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

void CompanionActor::draw() {
  assert(sprite != NULL);
  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);

  if (emote != NULL) {
    drawEmote();
  }
}

