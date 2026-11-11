#include <cassert>
#include <random>
#include <raylib.h>
#include "game.h"
#include "system/sprite_atlas.h"
#include "menu/hud/w_portrait.h"

using std::uniform_real_distribution;
SpriteAtlas WeaponPortrait::atlas("menu", "weapon_portraits");


WeaponPortrait::WeaponPortrait(Vector2 position, int sprite_id) {
  main_position = position;
  color = Game::palette[40];

  atlas.use();
  assert(sprite_id > 0);
  icon = &atlas.sprites[sprite_id];
}

WeaponPortrait::~WeaponPortrait() {
  atlas.release();
}

void WeaponPortrait::update() {
  if (!active) {
    return;
  }

  offset_clock += Game::deltaTime() / offset_time;
  if (offset_clock >= 1.0) {
    uniform_real_distribution<float> offset_range(0.0, 1.5);
    offset_x = offset_range(Game::RNG);
    offset_y = offset_range(Game::RNG);

    uniform_real_distribution<float> angle_range(0.0, 2.0);
    offset_angle = angle_range(Game::RNG);

    offset_direction = offset_direction * -1;
    offset_clock = 0.0;
  }
}

void WeaponPortrait::draw() {
  Rectangle dest = {main_position.x, main_position.y, 64, 112};
  DrawTexturePro(atlas.sheet, atlas.sprites[0], dest, {32, 56}, 0, color);

  float angle = 0;
  if (active) {
    dest.x += offset_x * offset_direction;
    dest.y += offset_y * offset_direction;
    angle = offset_angle * offset_direction;
  }

  DrawTexturePro(atlas.sheet, *icon, dest, {32, 56}, angle, color);
}
