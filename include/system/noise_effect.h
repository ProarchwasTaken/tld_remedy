#pragma once
#include <array>
#include <raylib.h>
#include "data/animation.h"


class NoiseEffect {
public:
  NoiseEffect();
  ~NoiseEffect();

  void setTint(Color tint);

  void setAlpha(float alpha);
  float getAlpha();

  void update();
  void draw();
private:
  std::array<Texture, 6> frames;
  Animation anim = {{0, 1, 2, 3, 4, 5}, 0.10};

  Color tint = WHITE;
  float alpha = 0.0;
};
