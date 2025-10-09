#include <raylib.h>
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include "menu/hud/reticle.h"


TargetReticle::TargetReticle() {
  menu_atlas = &CampMenuScene::menu_atlas;
  menu_atlas->use();
}

TargetReticle::~TargetReticle() {
  menu_atlas->release();
}

void TargetReticle::draw(Vector2 position) {
  Rectangle *sprite = &menu_atlas->sprites[7];
  Rectangle dest = *sprite;
  dest.x = position.x;
  dest.y = position.y;

  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {0, 0}, 0, WHITE);

  dest.x += 104;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {0, 0}, 90, WHITE);

  dest.x = position.x;
  dest.y += 54;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {0, 0}, -90, WHITE);

  dest.x += 104;
  DrawTexturePro(menu_atlas->sheet, *sprite, dest, {0, 0}, 180, WHITE);
}
