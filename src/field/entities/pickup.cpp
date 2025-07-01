#include <cassert>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "data/entity.h"
#include "data/field_event.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"
#include "field/entities/pickup.h"
#include <plog/Log.h>

SpriteAtlas Pickup::atlas("entities", "pickup_entity");


Pickup::Pickup(PickupData &data) {
  object_id = data.object_id;
  entity_type = EntityType::PICKUP;

  pickup_type = data.pickup_type;
  count = data.count;

  position = data.position;
  bounding_box.scale = {16, 16};
  bounding_box.offset = {-8, -8};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  atlas.use();
  animation = &anim_idle;
  PLOGI << "Entity Created: Pickup [ID: " << entity_id << "]";
}

Pickup::~Pickup() {
  atlas.release();
}

void Pickup::interact() {
  PLOGD << "Interaction function for Pickup [ID: " << entity_id << "]"
    " has been called.";

  switch (pickup_type) {
    case PickupType::SUPPLIES: {
      FieldHandler::raise<AddSuppliesEvent>(FieldEVT::ADD_SUPPLIES, 
                                            count);
    }
  }

  FieldHandler::raise<DeleteEntityEvent>(FieldEVT::DELETE_ENTITY, 
                                         entity_id);
  FieldHandler::raise<UpdateCommonEvent>(FieldEVT::UPDATE_COMMON_DATA, 
                                         object_id, false);
  interacted = true;
}

void Pickup::update() {
  if (interacted) {
    return;
  }

  SpriteAnimation::play(animation, NULL, true);

  bool inside = CheckCollisionPointRec(plr->position, bounding_box.rect);
  if (!in_range && inside) {
    ActorHandler::queue<ActorEvent>(this, ActorEVT::PICKUP_IN);
    in_range = true;
  }
  else if (in_range && !inside) { 
    ActorHandler::queue<ActorEvent>(this, ActorEVT::PICKUP_OUT);
    in_range = false;
  }
}

void Pickup::draw() {
  sprite = &atlas.sprites[*animation->current];

  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);
}
