#include <cmath>
#include <raylib.h>
#include "game.h"
#include "system/sprite_atlas.h"
#include "menu/hud/reticle.h"


TargetReticle::TargetReticle(Vector2 size) {
  this->size = size;

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();
}

TargetReticle::~TargetReticle() {
  menu_atlas->release();
}

void TargetReticle::draw(Vector2 position, float clock) {
  Rectangle *sprite = &menu_atlas->sprites[7];
  Rectangle dest = *sprite;
  dest.x = position.x;
  dest.y = position.y;

  float offset = std::sinf(clock * 15);

  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {-offset, -offset}, 
                 0, WHITE);

  dest.x += size.x;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {-offset, -offset}, 
                 90, WHITE);

  dest.x = position.x;
  dest.y += size.y;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {-offset, -offset}, 
                 -90, WHITE);

  dest.x += size.x;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {-offset, -offset}, 
                 180, WHITE);
}
