#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "base/actor.h"
#include "data/entity.h"
#include "data/field_event.h"
#include "field/system/field_handler.h"
#include "field/actors/player.h"
#include "field/entities/map_trans.h"

using std::string;


MapTransition::MapTransition(MapTransData &data) {
  entity_type = EntityType::MAP_TRANSITION;
  map_dest = data.map_dest;
  spawn_dest = data.spawn_dest;

  direction = data.direction;

  position = {data.rect.x, data.rect.y};
  bounding_box.scale = {data.rect.width, data.rect.height};
  bounding_box.offset = {0, 0};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  PLOGI << "Entity Created: Transition Trigger [ID: " 
    << entity_id << "]";
}

void MapTransition::update() {
  if (!plr->moving) {
    return;
  }

  if (plr->direction != direction) {
    return;
  }

  if (CheckCollisionPointRec(plr->position, bounding_box.rect)) {
    PLOGI << "Player has triggered Map Transition [ID: " << 
      entity_id << "]";
    FieldEventHandler::raise<LoadMapEvent>(LOAD_MAP, map_dest, 
                                           spawn_dest);
  }
}
