#include <cassert>
#include <cstddef>
#include "enums.h"
#include "base/actor.h"
#include "data/entity.h"
#include "field/actors/player.h"
#include "field/entities/pickup.h"
#include <plog/Log.h>


Pickup::Pickup(PickupData &data) {
  entity_type = EntityType::PICKUP;
  pickup_type = data.pickup_type;
  count = data.count;

  position = data.position;
  bounding_box.scale = {16, 16};
  bounding_box.offset = {-8, -8};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(plr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  PLOGI << "Entity Created: Pickup [ID: " << entity_id << "]";
}

void Pickup::update() {

}
