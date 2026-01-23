#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/personal.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "menu/panels/remap.h"
#include <plog/Log.h>


RemapPanel::RemapPanel(SpriteAtlas *menu_atlas, Settings *settings) {
  id = PanelID::REMAP;

  frame = LoadTexture("graphics/menu/remap_frame.png");

  atlas = menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  this->settings = settings; 
  this->keybinds = &Game::settings.menu_keybinds;
  PLOGI << "Control Remapping Panel: Initialized";
}

RemapPanel::~RemapPanel() {
  UnloadTexture(frame);
  atlas->release();
  sfx->release();
}

void RemapPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  optionNavigation();
}

void RemapPanel::heightLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 231 * percentage;
}

void RemapPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
    return;
  }
}

void RemapPanel::draw() {
  Rectangle source = {0, 0, 306, frame_height};
  DrawTextureRec(frame, source, main_position, WHITE);
}
