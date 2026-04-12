#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "base/actor.h"
#include "data/entity.h"
#include "data/session.h"
#include "data/field_event.h"
#include "utils/flag.h"
#include "scenes/field.h"
#include "field/system/field_handler.h"
#include "field/actors/player.h"
#include "field/entities/map_trans.h"

using std::string;


MapTransition::MapTransition(Session *session, MapTransData &data) {
  entity_type = EntityType::MAP_TRANSITION;
  map_dest = data.map_dest;
  spawn_dest = data.spawn_dest;

  direction = data.direction;
  required_flag = data.flag;

  position = {data.rect.x, data.rect.y};
  bounding_box.scale = {data.rect.width, data.rect.height};
  bounding_box.offset = {0, 0};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  this->session = session;

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

  if (!CheckCollisionPointRec(plr->position, bounding_box.rect)) {
    in_trigger = false;
    return;
  }

  if (!in_trigger) {
    PLOGI << "Player has triggered Map Transition [ID: " << 
    entity_id << "]";
    in_trigger = true;
  }
  else {
    return;
  }

  bool freely_usable = required_flag == FlagID::NONE;
  if (freely_usable || Flag::check(session, required_flag)) {
    FieldHandler::raise<LoadMapEvent>(FieldEVT::LOAD_MAP, map_dest, 
                                      spawn_dest);
    FieldScene::sfx.play("map_transition");
  }
  else {
    PLOGE << "Player does not meet the flag requirement to trigger this "
      << "Map Transition!";
    FieldHandler::raise<StartFSequenceEvent>(
      FieldEVT::START_FLAG_SEQUENCE, SequenceID::REJECT, required_flag
    );
  }
}
