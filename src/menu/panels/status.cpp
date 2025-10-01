#include <cassert>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "menu/panels/status.h"
#include <plog/Log.h>

using std::string;


StatusPanel::StatusPanel(SpriteAtlas *menu_atlas, MenuKeybinds *keybinds,
                         Session *session, string *description) 
{
  id = PanelID::STATUS;
  frame = LoadTexture("graphics/menu/status_frame.png");
  selected = options.begin();

  this->keybinds = keybinds;
  this->session = session;
  this->description = description;

  this->menu_atlas = menu_atlas;
  this->menu_atlas->use();
  sfx = &Game::menu_sfx;
  sfx->use();
  PLOGI << "Status Panel: Initialized.";
}

StatusPanel::~StatusPanel() {
  UnloadTexture(frame);
  menu_atlas->release();
  sfx->release();
}

void StatusPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  optionNavigation();
}

void StatusPanel::heightLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 144 * percentage;
}

void StatusPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void StatusPanel::draw() {
  Rectangle source = {0, 0, 160, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
}
