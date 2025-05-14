#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "base/actor.h"
#include "system/field_handler.h"
#include "data/field_event.h"
#include "actors/player.h"
#include "entities/map_trans.h"

using std::string;


MapTransition::MapTransition(string map_dest, string spawn_dest,
                             Rectangle rect, enum Direction direction)
{
  entity_type = EntityType::MAP_TRANSITION;
  this->map_dest = map_dest;
  this->spawn_dest = spawn_dest;

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  position = {rect.x, rect.y};
  bounding_box.scale = {rect.width, rect.height};
  bounding_box.offset = {0, 0};
  rectExCorrection(bounding_box);
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
