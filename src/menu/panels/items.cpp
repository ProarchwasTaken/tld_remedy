#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/input.h"
#include "scenes/camp_menu.h"
#include "menu/panels/items.h"
#include <plog/Log.h>

using std::string;


ItemsPanel::ItemsPanel(Session *session, string *description) {
  id = PanelID::ITEMS;
  frame = LoadTexture("graphics/menu/items_frame.png");

  this->session = session;
  this->description = description;

  this->keybinds = &Game::settings.menu_keybinds;

  this->sfx = &Game::menu_sfx;
  sfx->use();

  this->camp_atlas = &CampMenuScene::atlas;
  camp_atlas->use();

  this->menu_atlas = &CampMenuScene::menu_atlas;
  menu_atlas->use();
  PLOGI << "Items Panel: Initialized.";
}

ItemsPanel::~ItemsPanel() {
  UnloadTexture(frame);
  sfx->release();
  camp_atlas->release();
  menu_atlas->release();
}

void ItemsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  optionNavigation();
}

void ItemsPanel::heightLerp() {
  float percentage;

  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 104 * percentage;
}

void ItemsPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::draw() {
  Rectangle source = {0, 0, 200, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
}
