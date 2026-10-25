#include <cstddef>
#include <raylib.h>
#include "utils/animation.h"
#include "system/noise_effect.h"
#include <plog/Log.h>


NoiseEffect::NoiseEffect(Vector2 position, Vector2 scale) {
  this->position = position;

  for (int x = 0; x < frames.size(); x++) {
    Texture *texture = &frames.at(x);
    Image noise = GenImageWhiteNoise(scale.x, scale.y, 0.35);
    *texture = LoadTextureFromImage(noise);
    UnloadImage(noise);
  }
}

NoiseEffect::~NoiseEffect() {
  for (Texture &texture : frames) {
    UnloadTexture(texture);
  }

  PLOGI << "Noise Effect has been cleared from memory.";
}

void NoiseEffect::setTint(Color tint) {
  this->tint = tint;
}

void NoiseEffect::setAlpha(float alpha) {
  this->alpha = alpha;
}

float NoiseEffect::getAlpha() {
  return alpha;
}

void NoiseEffect::update() {
  if (alpha != 0.0) {
    Animation *animation = &anim;
    SpriteAnimation::play(animation, NULL, true);
  }
}

void NoiseEffect::draw() {
  if (alpha == 0.0) {
    return;
  }

  Texture *texture = &frames[*anim.current];
  tint.a = 255 * alpha;
  DrawTextureV(*texture, position, tint);
}
