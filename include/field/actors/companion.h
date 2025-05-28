#pragma once
#include <raylib.h>
#include <list>
#include "enums.h"
#include "base/actor.h"



class CompanionActor : public Actor {
public:
  CompanionActor(Vector2 position, enum Direction direction);

  void behavior() override;
  void processEvents();

  void update() override;
  void pathfind();

  void draw() override;
  void drawDebug() override;
private:
  std::list<Vector2> move_points;

  float movement_speed = 1.1;
};
