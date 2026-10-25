#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "utils/math.h"
#include "system/sprite_atlas.h"
#include "system/noise_effect.h"
#include "menu/hud/portrait.h"

using std::make_unique;
SpriteAtlas Portrait::atlas("menu", "portraits");


Portrait::Portrait(Vector2 position) {
  main_position = position;

  atlas.use();

  Vector2 noise_position = Vector2Add(main_position, {3, 3});
  Vector2 noise_scale = {58, 138};

  noise = make_unique<NoiseEffect>(noise_position, noise_scale);
  noise->setTint(Game::palette[7]);
  noise->setAlpha(0.05);
}

Portrait::~Portrait() {
  atlas.release();
  noise.reset();
}

void Portrait::update(float percentage) {
  portrait_height = 144 * percentage;

  if (percentage == 1.0 && fade_clock != 1.0) {
    fade_clock += Game::deltaTime() / fade_time;
    fade_clock = Clamp(fade_clock, 0.0, 1.0);
  }

  noise->update();
}

void Portrait::draw() {
  Rectangle source = *sprite;
  source.height = portrait_height;

  DrawTextureRec(atlas.sheet, source, main_position, WHITE);

  if (portrait_height == sprite->height) {
    noise->draw();
  }

  if (fade_clock != 1.0) {
    float percentage = 1.0 - fade_clock;
    Color white = WHITE;
    white.a = Math::smoothstep(0, 255, percentage);

    Rectangle rect = source;
    rect.x = main_position.x;
    rect.y = main_position.y;
    DrawRectangleRec(rect, white);
  }
}
