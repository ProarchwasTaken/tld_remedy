#pragma once
#include <memory>
#include <raylib.h>
#include "system/sprite_atlas.h"
#include "system/noise_effect.h"


class Portrait {
public:
  Portrait(Vector2 position);
  ~Portrait();
  void update(float percentage);
  void draw();

  static SpriteAtlas atlas;

  Vector2 main_position;
  Rectangle *sprite;
  std::unique_ptr<NoiseEffect> noise;
  float fade_clock = 0.0;
private:
  float portrait_height = 0;
  float fade_time = 0.25;
};
