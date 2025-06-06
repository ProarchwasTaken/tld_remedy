#pragma once
#include <raylib.h>
#include <list>
#include <utility>
#include "enums.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"



class CompanionActor : public Actor {
public:
  CompanionActor(Vector2 position, enum Direction direction);
  ~CompanionActor();

  void behavior() override;
  void processEvents();

  void update() override;
  void pathfind();

  void draw() override;
  Rectangle *getIdleSprite();
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  std::list<std::pair<Vector2, enum Direction>> move_points;
  float movement_speed = 1.1;
};
