#pragma once
#include <string>
#include "enums.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"


class FieldSequence {
public:
  FieldSequence(std::string name, SequenceID id);
  virtual ~FieldSequence();

  virtual void update() = 0;

  std::string name;
  SequenceID id;

  int order = 0;
  bool end_sequence = false;
protected:
  PlayerActor *player;
  CompanionActor *companion;
};
