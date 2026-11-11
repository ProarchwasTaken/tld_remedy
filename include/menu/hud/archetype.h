#pragma once
#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"


class Archetype {
public:
  Archetype(Vector2 position);
  ~Archetype();

  void setIcon(CompanionID id);
  void draw();

  float source_height = 70;
private:
  static SpriteAtlas atlas;
  Rectangle *icon;

  Vector2 position;
};
