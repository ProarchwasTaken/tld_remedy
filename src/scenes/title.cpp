#include <cassert>
#include <cmath>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include "enums.h"
#include "game.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "menu/panels/config.h"
#include "menu/panels/confirm.h"
#include "scenes/title.h"
#include <plog/Log.h>

using std::string, std::make_unique;


TitleScene::TitleScene() {
  scene_id = SceneID::TITLE;
  keybinds = &Game::settings.menu_keybinds;

  sfx = &Game::menu_sfx;
  sfx->use();
  menu_atlas.use();

  valid_session = Game::existingSession();
  if (!valid_session) {
    disallowed.emplace(TitleOption::LOAD_GAME);
  }
}

TitleScene::~TitleScene() {
  sfx->release();
  menu_atlas.release();

  if (panel != nullptr) {
    panel.reset();
  }
}

void TitleScene::update() {
  if (!panel_mode) {
    blink_clock += Game::deltaTime();
    optionNavigation();
    return;
  }

  assert(panel != nullptr);
  panel->update();

  if (panel->terminate) {
    panel.reset();
    panel_mode = false;
  }
}

void TitleScene::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    panel = make_unique<ConfirmPanel>(&menu_atlas, keybinds, 
                                     "Close the game?");
    sfx->play("menu_cancel");
    panel_mode = true;
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
      panel = make_unique<ConfigPanel>(&menu_atlas, keybinds);
      panel_mode = true;
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

  if (panel_mode) {
    panel->draw();
    assert(panel != nullptr);
  }
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
  if (!panel_mode) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
  }

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
