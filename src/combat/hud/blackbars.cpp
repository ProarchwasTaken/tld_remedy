#include <cassert>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "utils/math.h"
#include "combat/system/camera.h"
#include "combat/hud/blackbars.h"
#include <plog/Log.h>


BlackBars::BlackBars() {
  texture = LoadTexture("graphics/hud/blackbar.png");
  tint.a = 225;
}

BlackBars::~BlackBars() {
  UnloadTexture(texture);
}

void BlackBars::update(CombatCamera *camera) {
  top_dest.x = camera->target.x;
  bottom_dest.x = camera->target.x;
}

void BlackBars::draw() {
  if (speed != target_speed) {
    speedLerp();
  }

  source.x += speed * Game::deltaTime();

  if (source.x > 16) {
    source.x = source.x - 16;
  }

  DrawTexturePro(texture, source, top_dest, {215, 0}, 178, tint);
  DrawTexturePro(texture, source, bottom_dest, {215, 0}, -2, tint);
}

void BlackBars::speedLerp() {
  assert(speed != target_speed);
  decel_clock += Game::deltaTime() / decel_time;
  decel_clock = Clamp(decel_clock, 0.0, 1.0);

  speed = Math::smoothstep(set_speed, target_speed, decel_clock);

  if (decel_clock == 1.0) {
    decel_clock = 0.0;
  }
}
