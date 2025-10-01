#include <cmath>
#include <cassert>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include "menu/panels/status.h"
#include <plog/Log.h>

using std::string;


StatusPanel::StatusPanel(Session *session, string *description) 
{
  id = PanelID::STATUS;
  frame = LoadTexture("graphics/menu/status_frame.png");

  options[0] = &session->player;
  options[1] = &session->companion;
  selected = options.begin();

  this->keybinds = &Game::settings.menu_keybinds;
  this->description = description;

  menu_atlas = &CampMenuScene::menu_atlas;
  menu_atlas->use();

  camp_atlas = &CampMenuScene::atlas;
  camp_atlas->use();
  sfx = &Game::menu_sfx;
  sfx->use();
  PLOGI << "Status Panel: Initialized.";
}

StatusPanel::~StatusPanel() {
  UnloadTexture(frame);
  menu_atlas->release();
  camp_atlas->release();
  sfx->release();
}

void StatusPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  blink_clock += Game::deltaTime();
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

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void StatusPanel::draw() {
  Rectangle source = {0, 0, 160, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  drawOptions();
}

void StatusPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  Rectangle *sprite = &camp_atlas->sprites[0];

  for (int index = 0; index < 2; index++) {
    Character *party_member = options[index];
    PartyMemberID id = party_member->member_id;

    Vector2 position = option_position;
    position.y += 16 * index;

    DrawTextureRec(camp_atlas->sheet, *sprite, position, WHITE);

    Character *current = *selected;
    if (current->member_id == id) {
      drawCursor(position);
    }

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, party_member->name, position, txt_size, -2, WHITE);
  }
}

void StatusPanel::drawCursor(Vector2 position) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;
  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}
