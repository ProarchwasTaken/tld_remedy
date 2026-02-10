#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/math.h"
#include "scenes/rest_menu.h"

using std::string, std::uniform_int_distribution;


RestMenuScene::RestMenuScene(Session *session) {
  assert(session != NULL);
  scene_id = SceneID::REST_MENU;
  this->session = session;

  loadBackground(session->location);
  camera.target = {0, 0};
  camera.offset = {213, 120};
  camera.rotation = 0;
  camera.zoom = 0.85;

  black_bars.enabled = false;
  black_bars.setTargetValues(5, 8);
  black_bars.setValues(20, -42);

  atlas.use();
  setupCharacters();
}

RestMenuScene::~RestMenuScene() {
  UnloadTexture(background);
  atlas.release();
}

void RestMenuScene::loadBackground(string location) {
  if (location == "Cathedral" || location == "Debug") {
    background = LoadTexture("graphics/menu/backgrounds/cathedral.png");
    bg_color = Game::palette[40];
    bg_color.a = 0.0;

    char_color = Game::palette[42];
    char_color.a = 0.0;

    black_bars.setTint(Game::palette[43]);
  }
}

void RestMenuScene::setupCharacters() {
  plr_sprite = &atlas.sprites[1];
  plr_position = {59, -21};

  switch (session->companion.companion_id) {
    case CompanionID::ERWIN: {
      com_sprite = &atlas.sprites[2];
      com_position = {4, -21};
    }
  }
}

void RestMenuScene::update() {
  switch (state) {
    case OPENING: {
      openingLogic();
      break;
    }
    case READY: {
      // Remove this later!
      if (IsKeyPressed(KEY_K)) {
        state = OPENING;
      }

      flickeringLogic();
      break;
    }
  }

  float now = GetTime();
  float offset_x = std::cosf(now * 0.5) * 8;
  float offset_y = std::sinf(now) * 4;
  camera.target = {offset_x, offset_y};

  black_bars.update(&camera);
}

void RestMenuScene::openingLogic() {
  state_clock += Game::deltaTime() / state_time;
  state_clock = Clamp(state_clock, 0.0, 1.0);

  if (!black_bars.enabled && state_clock >= 0.60) {
    black_bars.enabled = true;
  }

  float percentage = Clamp((-0.5 + state_clock) / 0.25, 0.0, 1.0);
  bg_color.a = 255 * percentage;
  char_color.a = 255 * percentage;

  percentage = Clamp((-0.5 + state_clock) / 0.50, 0.0, 1.0);
  camera.zoom = Math::smoothstep(0.85, 1.0, percentage);

  if (state_clock == 1.0) {
    state = READY;
    state_clock = 0.0;
  }
}

void RestMenuScene::flickeringLogic() {
  tick_clock += Game::deltaTime() / tick_time;
  if (tick_clock < 1.0) {
    return;
  }

  tick_clock = 1.0;

  bool end = false;
  if (char_color.a != 255) {
    char_color.a = 255;
    end = true;
  }

  if (bg_color.a != 255) {
    bg_color.a = 255;
    end = true;
  }

  if (end) {
    return;
  }

  uniform_int_distribution<int> range(1, 50);
  int number = range(Game::RNG);
  if (number == 50) {
    char_color.a = 200;
  }

  number = range(Game::RNG);
  if (number < 2) {
    bg_color.a = 128;
  }
}

void RestMenuScene::draw() {
  BeginMode2D(camera);
  { 
    DrawTextureV(background, {-250, -150}, bg_color);
    black_bars.draw();
    DrawTextureRec(atlas.sheet, *plr_sprite, plr_position, char_color);
    DrawTextureRec(atlas.sheet, *com_sprite, com_position, char_color);
  }
  EndMode2D();
}
