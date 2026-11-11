#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"


class StarRating {
public:
  StarRating(Vector2 position, Color color);
  ~StarRating();

  void draw();

  int rating = 0;
private:
  SpriteAtlas *atlas;
  Vector2 base_position;
  Color color;
};
