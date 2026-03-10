#include <cassert>
#include <cstddef>
#include <vector>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "data/entity.h"
#include "data/session.h"
#include "data/field_event.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "utils/items.h"
#include "utils/flag.h"
#include "scenes/field.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"
#include "field/entities/pickup.h"
#include <plog/Log.h>

SpriteAtlas Pickup::atlas("entities", "pickup_entity");
using std::vector, std::string;


Pickup::Pickup(Session *session, PickupData &data) {
  object_id = data.object_id;
  entity_type = EntityType::PICKUP;

  this->session = session;
  pickup_type = data.pickup_type;
  value = data.value;

  position = data.position;
  bounding_box.scale = {16, 16};
  bounding_box.offset = {-8, -8};
  rectExCorrection(bounding_box);

  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  assert(ptr != NULL);
  plr = static_cast<PlayerActor*>(ptr);

  atlas.use();

  if (pickup_type != PickupType::SUPPLIES) {
    anim_idle = {{3, 4, 5}, 0.10};
  }

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
                                            value);
      break;
    }
    case PickupType::ITEM: {
      ItemID item = static_cast<ItemID>(value);
      vector<string> dialog;
      string name = ItemUtils::getShortened(item);

      if (session->item_count < session->item_limit) {
        FieldHandler::raise<AddItemEvent>(FieldEVT::ADD_ITEM, item);

        dialog = {"Obtained <" + name + ">."};
        FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, 
                                             dialog);
      }
      else {
        dialog = {
          "Mary finds a <" + name + ">, but he\n"
          "lacks the inventory space to take it\n"
          "with him."
        };
        FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, 
                                             dialog);
        return;
      }

      break;
    }
    case PickupType::FLAG: {
      FlagID flag = static_cast<FlagID>(value);

      PLOGI << "Attempting to raise Flag of ID: " << value;
      Flag::set(session, flag, true);
      openFlagDialog(flag);
      break;
    }
  }

  FieldHandler::raise<DeleteEntityEvent>(FieldEVT::DELETE_ENTITY, 
                                         entity_id);
  FieldHandler::raise<UpdateCommonEvent>(FieldEVT::UPDATE_COMMON_DATA, 
                                         object_id, false);

  FieldScene::sfx.play("pickup");
  interacted = true;
}

void Pickup::openFlagDialog(FlagID flag) {
  vector<string> dialog;

  switch (flag) {
    case FlagID::CDF1_KEY: {
      dialog = {"Obtained <Entrance Key>"};
      break;
    }
    default: {
      PLOGE << "There's no dialog availiable for flag of ID: " << value;
      return;
    }
  }

  FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog);
}

void Pickup::update() {
  if (interacted) {
    return;
  }

  SpriteAnimation::play(animation, NULL, true);

  bool inside = CheckCollisionPointRec(plr->position, bounding_box.rect);
  if (!in_range && inside) {
    ActorHandler::queue<ActorEvent>(this, ActorEVT::INTERACT_IN);
    in_range = true;
  }
  else if (in_range && !inside) { 
    ActorHandler::queue<ActorEvent>(this, ActorEVT::INTERACT_OUT);
    in_range = false;
  }
}

void Pickup::draw() {
  sprite = &atlas.sprites[*animation->current];

  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);
}
