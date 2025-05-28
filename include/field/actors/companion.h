#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"



class CompanionActor : public Actor {
public:
  CompanionActor(Vector2 position, enum Direction direction);

  void behavior() override;

  void update() override;

  void draw() override;
};
