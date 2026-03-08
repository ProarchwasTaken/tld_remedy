#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "enums.h"
#include "system/sprite_atlas.h"
#include "base/actor.h"

using std::string;
SpriteAtlas Actor::emotes("entities", "emote_balloons");


Actor* Actor::getActor(enum ActorType type) {
  Actor *result = NULL;

  for (Actor* actor : existing_actors) {
    if (actor->actor_type == type) {
      PLOGD << "Actor Found: '" << actor->name << "' [ID: " 
        << actor->entity_id << "]";
      result = actor;
      break;
    }
  }

  return result;
}

Actor::Actor(string name, enum ActorType actor_type, Vector2 position, 
             enum Direction direction) 
{
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::ACTOR;
  this->actor_type = actor_type;
  bool successful = existing_actors.emplace(this).second;

  assert(successful);
  emotes.use();
  PLOGI << "ACTOR: '" << name << "' [ID: " << entity_id << "]";
}

Actor::~Actor() {
  int erased = existing_actors.erase(this);
  assert(erased == 1);

  emotes.release();
  PLOGI << "Removed actor: '" << name << "'";
}

void Actor::drawEmote() {
  assert(emote != NULL);
  Rectangle dest = {position.x, bounding_box.position.y, 16, 16};
  DrawTexturePro(emotes.sheet, *emote, dest, {8, 16}, 0, WHITE);
}

void Actor::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(collis_box.rect, 1, RED);
}
