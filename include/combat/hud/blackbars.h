#pragma once
#include <raylib.h>
#include "combat/system/camera.h"


class BlackBars {
public:
  BlackBars();
  ~BlackBars();

  void behavior();
  void update(CombatCamera *camera);

  void draw();
  void speedLerp();

private:
  Vector2 top_position = {213, 24};
  Vector2 bottom_position = {213, 216};

  Texture texture;
  Color tint = WHITE;
  Rectangle source = {0, 0, 430, 80};

  Rectangle top_dest = {0, 24, 430, 80};
  Rectangle bottom_dest = {0, 216, 430, 80};

  static constexpr float DEFAULT_SPEED = 10;
  float speed = DEFAULT_SPEED;
  float set_speed = speed;
  float target_speed = speed;

  float decel_clock = 0.0;
  float decel_time = 0.5;
};
