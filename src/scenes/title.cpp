#include <cassert>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include "enums.h"
#include "game.h"
#include "utils/input.h"
#include "scenes/title.h"
#include <plog/Log.h>

using std::string;


TitleScene::TitleScene() {
  scene_id = SceneID::TITLE;
  keybinds = &Game::settings.menu_keybinds;

  sfx = &Game::menu_sfx;
  sfx->use();
  menu_atlas.use();

  valid_session = Game::existingSession();
}

TitleScene::~TitleScene() {
  sfx->release();
  menu_atlas.release();
}

void TitleScene::update() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    nextOption();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    prevOption();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
    sfx->play("menu_select");
  }
}

void TitleScene::nextOption() {
  selected++;

  if (selected == options.end()) {
    selected = options.begin();
  }

  if (!valid_session && *selected == TitleOption::LOAD_GAME) {
    selected++;
  }
}

void TitleScene::prevOption() {
  if (selected == options.begin()) {
    selected = options.end();
  }

  selected--;

  if (!valid_session && *selected == TitleOption::LOAD_GAME) {
    selected--;
  }
}

void TitleScene::selectOption() {
  switch (*selected) {
    case TitleOption::NEW_GAME: {
      Game::newSession(SubWeaponID::KNIFE, CompanionID::ERWIN);
      break;
    }
    case TitleOption::LOAD_GAME: {
      assert(valid_session);
      Game::loadSession();
      break;
    }
    case TitleOption::CONFIG: {
      PLOGE << "This doesn't do anything yet!";
      break;
    }
  }
}

void TitleScene::draw() {
  Font *font = &Game::med_font;
  int txt_size = Game::med_font.baseSize;

  DrawTextEx(*font, "True Human Tribulation III: Remedy", {8, 8}, 
             txt_size, -2, Game::palette[14]);
  drawOptions(font, txt_size);
}

void TitleScene::drawOptions(Font *font, int txt_size) {
  Vector2 position = {16, 180};

  for (TitleOption id : options) {
    string text = getOptionText(id);

    Color color = WHITE;
    if (!valid_session && id == TitleOption::LOAD_GAME) {
      color = Game::palette[2];
    }
    else if (*selected == id) {
      color = Game::palette[14];
      drawCursor(position, color);
    }

    DrawTextEx(*font, text.c_str(), position, txt_size, -2, color);
    position.y += 16;
  }
}

void TitleScene::drawCursor(Vector2 position, Color color) {
  float sin_a = std::sinf(GetTime() * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  position = Vector2Add(position, {-11, 2});
  DrawTextureRec(menu_atlas.sheet, menu_atlas.sprites[0], position, 
                 color);
}

string TitleScene::getOptionText(TitleOption id) {
  switch (id) {
    case TitleOption::NEW_GAME: {
      return "NEW GAME";
    }
    case TitleOption::LOAD_GAME: {
      return "LOAD GAME";
    }
    case TitleOption::CONFIG: {
      return "CONFIG";
    }
  }
}
