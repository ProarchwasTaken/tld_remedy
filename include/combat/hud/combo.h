#pragma once
#include <raylib.h>
#include "system/sprite_atlas.h"


class ComboHud {
public:
  ComboHud(Vector2 position);
  ~ComboHud();

  void behavior();
  void update();

  void stunTimer();
  void startComboToast();

  void draw();
  void drawComboCount(int value);

  static SpriteAtlas atlas;
private:
  Vector2 main_position;
  Color tint = WHITE;

  float end_clock = 1.0;
  float end_time = 1.0;

  float stun_clock = 1.0;
  float stun_time = 0.0;

  float hit_clock = 1.0;
  float hit_time = 0.10;
};
