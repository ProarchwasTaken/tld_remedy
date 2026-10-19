#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"


class WeaponPortrait {
public:
  WeaponPortrait(Vector2 position, int sprite_id);
  ~WeaponPortrait();

  void update();
  void draw();

  bool active = false;
  Vector2 main_position;
  Color color;
private:
  static SpriteAtlas atlas;
  Rectangle *icon;

  float offset_clock = 0.0;
  float offset_time = 1.0;

  float offset_x = 0;
  float offset_y = 0;
  float offset_angle = 0;
  int offset_direction = 1;
};

