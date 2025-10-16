#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/entity.h"
#include "system/sprite_atlas.h"


class AfterImage : public Entity {
public:
  AfterImage(SpriteAtlas *atlas, Rectangle *sprite, Vector2 position,
             Direction Direction, float life_time = 0.25, 
             Color tint = WHITE);
  ~AfterImage();

  void update() override;
  void draw() override;
private:
  SpriteAtlas *atlas;
  Color tint;

  Direction direction;

  float life_time;
  float life_clock = 0.0;
};
