#pragma once
#include <cstddef>
#include <raylib.h>
#include "system/sprite_atlas.h"


class CombatToasts {
public:
  CombatToasts(Vector2 position);
  ~CombatToasts();

  void startToast(int toast_id);

  void draw();
  void drawToast();

  static SpriteAtlas atlas;
private:
  Vector2 main_position;
  Rectangle *sprite = NULL;

  float toast_time = 0.80;
  float toast_clock = 0.0;
};
