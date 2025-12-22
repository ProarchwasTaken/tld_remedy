#include <cmath>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "system/sprite_atlas.h"
#include "combat/hud/toasts.h"
#include <plog/Log.h>

SpriteAtlas CombatToasts::atlas("hud", "hud_toasts");


CombatToasts::CombatToasts(Vector2 position) {
  this->main_position = position;
  atlas.use();
}

CombatToasts::~CombatToasts() {
  atlas.release();
}

void CombatToasts::startToast(int toast_id) {
  sprite = &atlas.sprites.at(toast_id);
  toast_clock = 0.0;
}

void CombatToasts::draw() {
  if (sprite == NULL) {
    return;
  }

  toast_clock += Game::deltaTime() / toast_time;
  toast_clock = Clamp(toast_clock, 0.0, 1.0);

  drawToast();

  if (toast_clock == 1.0) {
    sprite = NULL;
    PLOGI << "Finished toast.";
  }
}

void CombatToasts::drawToast() {
  Rectangle dest = *sprite;
  dest.x = main_position.x;
  dest.y = main_position.y;
  
  float percentage = Clamp(toast_clock / 0.10, 0.0, 1.0);
  dest.height = dest.height * percentage;
  float offset = dest.height / 2;

  percentage = Clamp((-0.70 + toast_clock) / 0.30, 0.0, 1.0);
  percentage = 1.0 - percentage;

  Rectangle source = *sprite;
  source.width = source.width * percentage;
  dest.width = source.width;

  float remaining = sprite->width - source.width;
  source.x += remaining;
  dest.x += remaining;

  DrawTexturePro(atlas.sheet, source, dest, {0, offset}, 0, WHITE);
}
