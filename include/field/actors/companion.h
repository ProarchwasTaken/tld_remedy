#pragma once
#include <raylib.h>
#include <list>
#include <utility>
#include "enums.h"
#include "base/actor.h"
#include "data/animation.h"
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
  Rectangle *getWalkSprite();
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  std::list<std::pair<Vector2, enum Direction>> move_points;
  float movement_speed = 1.1;

  Animation *animation = NULL;
  Animation anim_down = {{0, 1, 2, 1}, 0.2};
  Animation anim_right = {{3, 4, 5, 4}, 0.2};
  Animation anim_up = {{6, 7, 8, 7}, 0.2};
  Animation anim_left = {{9, 10, 11, 10}, 0.2};
};
