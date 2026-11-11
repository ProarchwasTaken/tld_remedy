#include <cassert>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/entity.h"
#include "data/field_event.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "utils/math.h"
#include "scenes/field.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"
#include "field/entities/save_point.h"
#include <plog/Log.h>

SpriteAtlas SavePoint::atlas("entities", "savepoint_entity");


SavePoint::SavePoint(SavePointData &data) {
  entity_type = EntityType::SAVE_POINT;
  rest_point = data.rest_point;

  position = data.position;
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -16};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  sfx = &FieldScene::sfx;
  sfx->use();

  atlas.use();
  sprite = &atlas.sprites[0];
  alt_sprite = &atlas.sprites[4];
  PLOGI << "Entity Created: SavePoint [ID: " << entity_id << "]";
}

SavePoint::~SavePoint() {
  atlas.release();
  sfx->release();
}

void SavePoint::interact() {
  PLOGD << "Interaction function for SavePoint [ID: " << entity_id << "]"
  " has been called.";
  if (!rest_point) {
    FieldHandler::raise<FieldEvent>(FieldEVT::SAVE_SESSION);
  }
  else {
    FieldHandler::raise<StartSequenceEvent>(FieldEVT::START_SEQUENCE,
                                            SequenceID::REST);
  }

  sfx->play("save_point_interact");
}

void SavePoint::update() {
  idleAnimation();
  proximityCheck();
}

void SavePoint::idleAnimation() {
  if (spin_clock < 1.0) {
    anim_idle.frame_duration = Math::smoothstep(0.01, 0.20, spin_clock);

    spin_clock += Game::deltaTime() / spin_time;
    spin_clock = Clamp(spin_clock, 0.0, 1.0);
  }

  SpriteAnimation::play(animation, &anim_idle, true);
  sprite = &atlas.sprites[*animation->current];

  if (rest_point) {
    int index = *animation->current + 4;
    alt_sprite = &atlas.sprites[index];
  }
}

void SavePoint::proximityCheck() {
  bool inside = CheckCollisionPointRec(plr->position, bounding_box.rect);
  if (!in_range && inside) {
    ActorHandler::queue<ActorEvent>(this, ActorEVT::INTERACT_IN);
    in_range = true;
    spin_clock = 0.0;
    sfx->play("save_point");
  }
  else if (in_range && !inside) { 
    ActorHandler::queue<ActorEvent>(this, ActorEVT::INTERACT_OUT);
    in_range = false;
  }
}

void SavePoint::draw() {
  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);

  if (rest_point) {
    DrawTexturePro(atlas.sheet, *alt_sprite, bounding_box.rect, {0, 0}, 
                   0, WHITE);
  }
}
