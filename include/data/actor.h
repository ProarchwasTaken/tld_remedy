#pragma once
#include <raylib.h>
#include <vector>
#include "data/entity.h"
#include "enums.h"


struct ActorData : EntityData {
  ActorType actor_type;
  Vector2 position;
  Direction direction;
};


struct EnemyActorData : ActorData {
  std::vector<Direction> routine;
  float speed;
};
