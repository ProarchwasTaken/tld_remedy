#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"

class TargetReticle {
public:
  TargetReticle();
  ~TargetReticle();

  void draw(Vector2 position);
private:
  SpriteAtlas *menu_atlas;
};
