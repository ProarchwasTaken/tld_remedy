#include <cassert>
#include <cstddef>
#include <string>
#include "enums.h"
#include "base/actor.h"
#include "base/field_sequence.h"
#include <plog/Log.h>

using std::string;


FieldSequence::FieldSequence(string name, SequenceID id) {
  this->name = name;
  this->id = id;

  Actor *actor = Actor::getActor(ActorType::PLAYER);
  assert(actor != NULL);
  player = static_cast<PlayerActor*>(actor);
  player->setControllable(false);

  actor = Actor::getActor(ActorType::COMPANION);
  assert(actor != NULL);
  companion = static_cast<CompanionActor*>(actor);
  PLOGI << "Starting Field Sequence: '" << name << "'";
}

FieldSequence::~FieldSequence() {
  player->setControllable(true);
  PLOGI << "Ending Field Sequence: '" << name << "'";
}
