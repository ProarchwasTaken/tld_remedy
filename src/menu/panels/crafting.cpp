#include <raylib.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "menu/panels/crafting.h"
#include <plog/Log.h>


CraftingPanel::CraftingPanel(Session *session, SpriteAtlas *rest_atlas) {
  id = PanelID::CRAFTING;
  frame = LoadTexture("graphics/menu/frames/crafting.png");

  this->session = session;
  keybinds = &Game::settings.menu_keybinds;

  this->rest_atlas = rest_atlas;

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();
  PLOGD << "CraftingPanel has been initialized.";
}

CraftingPanel::~CraftingPanel() {
  UnloadTexture(frame);
  menu_atlas->release();
  sfx->release();
}

void CraftingPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else {
    menuNavigation();
  }
}

void CraftingPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 168 * percentage;
}

void CraftingPanel::menuNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::draw() {
  Rectangle source = {0, 0, 296, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
}
