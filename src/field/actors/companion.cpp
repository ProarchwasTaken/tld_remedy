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
#include "data/rect_ex.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "field/actors/companion.h"

using std::unique_ptr, std::string;
SpriteAtlas CompanionActor::atlas("actors", "");
RectEx CompanionActor::DEF_BOUNDS;
RectEx CompanionActor::DEF_COLLIS;


CompanionActor::CompanionActor(Vector2 position, Direction direction):
  Actor("Companion", ActorType::COMPANION, position, direction)
{
  default_speed = 56.5;
  movement_speed = default_speed;

  bounding_box = DEF_BOUNDS;
  collis_box = DEF_COLLIS;
  rectExCorrection(bounding_box, collis_box);
  
  atlas.use();
  setIdleSprite();
}

CompanionActor::~CompanionActor() {
  atlas.release();
}

void CompanionActor::setAtlas(CompanionID id) {
  assert(atlas.users() == 0);
  string sprite_group;

  switch (id) {
    case CompanionID::ERWIN: {
      sprite_group = "erwin_actor";

      DEF_BOUNDS.scale = {32, 34};
      DEF_BOUNDS.offset = {-16, -30};
      DEF_COLLIS.scale = {8, 16};
      DEF_COLLIS.offset = {-4, -12};
      break;
    }
    case CompanionID::XANDER: {
      sprite_group = "xander_actor";

      DEF_BOUNDS.scale = {80, 64};
      DEF_BOUNDS.offset = {-40, -60};
      DEF_COLLIS= {24, 32};
      DEF_COLLIS.offset = {-12, -28};
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
  obscuring_player = playerObscured();

  if (!moving) {
    setIdleSprite();
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

bool CompanionActor::playerObscured() {
  if (plr == NULL || position.y <= plr->position.y) {
    return false;
  }

  Rectangle *rect = &collis_box.rect;
  Rectangle *plr_rect = &plr->collis_box.rect;

  if (CheckCollisionRecs(*rect, *plr_rect)) {
    Rectangle collision = GetCollisionRec(*rect, *plr_rect);

    float plr_area = plr_rect->width * plr_rect->height;
    float collision_area = collision.width * collision.height;
    return collision_area / plr_area >= 0.8;
  }
  else {
    return false; 
  }
}

void CompanionActor::setIdleSprite() {
  if (lock_sprite) {
    return;
  }

  animation = NULL;
  switch (direction) {
    case DOWN: {
      sprite = &atlas.sprites[1];
      return;
    }
    case RIGHT: {
      sprite = &atlas.sprites[4];
      return;
    }
    case UP: {
      sprite = &atlas.sprites[7];
      return;
    }
    case LEFT: {
      sprite = &atlas.sprites[10];
      return;
    }
  }
}

void CompanionActor::moveAnimation() {
  if (lock_sprite) {
    return;
  }

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

  float difference = 1.0 - (movement_speed / default_speed);
  float percentage = 1.0 + difference;
  next_anim->frame_duration = anim_move_speed * percentage;

  SpriteAnimation::play(animation, next_anim, true);
  sprite = &atlas.sprites[*animation->current];
}

void CompanionActor::draw() {
  assert(sprite != NULL);
  Color tint = WHITE;
  if (follow_player && obscuring_player) {
    tint.a = 200;
  }

  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 tint);

  if (emote != NULL) {
    drawEmote();
  }
}

