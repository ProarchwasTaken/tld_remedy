#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"


class EnemyActor : public Actor {
public:
  EnemyActor(Vector2 position, enum Direction direction);
  ~EnemyActor();

  void behavior() override;
  
  void update() override;
  Rectangle *getIdleSprite();

  void draw() override;

  static SpriteAtlas atlas;
};
