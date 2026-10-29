#include <raylib.h>
#include "scenes/setup.h"
#include "menu/hud/rating.h"


StarRating::StarRating(Vector2 position, Color color) {
  base_position = position;
  this->color = color;

  atlas = &SetupScene::atlas;
  atlas->use();
}

StarRating::~StarRating() {
  atlas->release();
}

void StarRating::draw() {
  int stars = rating / 2;
  Vector2 position = base_position;
  for (int x = 0; x < stars; x++) {
    DrawTextureRec(atlas->sheet, atlas->sprites[4], position, color);
    position.x += 12;
  }

  int leftover = rating % 2;
  if (leftover != 0) {
    DrawTextureRec(atlas->sheet, atlas->sprites[5], position, color);
  }
}
