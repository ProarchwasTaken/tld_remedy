#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"

class TargetReticle {
public:
  TargetReticle();
  ~TargetReticle();

  void draw(Vector2 position, float clock);
private:
  SpriteAtlas *menu_atlas;
};
