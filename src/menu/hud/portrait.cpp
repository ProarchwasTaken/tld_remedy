#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "utils/math.h"
#include "system/sprite_atlas.h"
#include "menu/hud/portrait.h"

SpriteAtlas Portrait::atlas("menu", "portraits");


Portrait::Portrait(Vector2 position) {
  this->position = position;
  atlas.use();
}

Portrait::~Portrait() {
  atlas.release();
}

void Portrait::update(float percentage) {
  portrait_height = 144 * percentage;

  if (percentage == 1.0 && fade_clock != 1.0) {
    fade_clock += Game::deltaTime() / fade_time;
    fade_clock = Clamp(fade_clock, 0.0, 1.0);
  }
}

void Portrait::draw(PartyMemberID member_id) {
  int index = static_cast<int>(member_id);
  Rectangle sprite = atlas.sprites.at(index);
  sprite.height = portrait_height;

  DrawTextureRec(atlas.sheet, sprite, position, WHITE);

  if (fade_clock != 1.0) {
    float percentage = 1.0 - fade_clock;
    Color white = WHITE;
    white.a = Math::smoothstep(0, 255, percentage);

    Rectangle rect = sprite;
    rect.x = position.x;
    rect.y = position.y;
    DrawRectangleRec(rect, white);
  }
}
