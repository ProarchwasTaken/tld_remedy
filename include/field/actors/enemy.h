#pragma once
#include <raylib.h>
#include <vector>
#include "enums.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"


class EnemyActor : public Actor {
public:
  EnemyActor(Vector2 position, std::vector<Direction> routine, 
             float speed);
  ~EnemyActor();

  void behavior() override;
  
  void update() override;
  void directionRoutine();
  Rectangle *getIdleSprite();

  void draw() override;

  static SpriteAtlas atlas;
private:
  std::vector<Direction> routine;
  std::vector<Direction>::iterator current_direction;
  float routine_clock = 0.0;

  float speed;
};
