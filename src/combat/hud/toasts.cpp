#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "system/sprite_atlas.h"
#include "combat/hud/toasts.h"

SpriteAtlas CombatToasts::atlas("hud", "hud_toasts");


CombatToasts::CombatToasts(Vector2 position) {
  this->main_position = position;
  atlas.use();
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

  if (toast_clock == 1.0) {
    sprite = NULL;
  }
}
