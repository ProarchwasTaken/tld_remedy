#include <cassert>
#include <cctype>
#include <cstddef>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/text.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "system/sprite_atlas.h"
#include "menu/panels/diagnose.h"
#include <plog/Log.h>

using std::string;


DiagnosePanel::DiagnosePanel(Session *session, SpriteAtlas *rest_atlas) {
  id = PanelID::DIAGNOSE;
  frame = LoadTexture("graphics/menu/frames/diagnose.png");
  keybind = &Game::settings.menu_keybinds;

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  assert(rest_atlas != NULL);
  this->rest_atlas = rest_atlas;

  sfx = &Game::menu_sfx;
  sfx->use();

  party = {&session->player, &session->companion};
  current_member = party.begin();
  PLOGD << "DiagnosePanel has been initialized.";
}

DiagnosePanel::~DiagnosePanel() {
  menu_atlas->release();
  sfx->release();
}

void DiagnosePanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else {
    menuNavigation();
  }

  portrait.update(percentage);
}

void DiagnosePanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 192 * percentage;
}

void DiagnosePanel::menuNavigation() {
  if (portrait.fade_clock != 1.0) {
    return;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybind->right, gamepad)) {
    MenuUtils::nextOption(party, current_member);
    sfx->play("menu_navigate");
    portrait.fade_clock = 0.0;
  }
  else if (Input::pressed(keybind->left, gamepad)) {
    MenuUtils::prevOption(party, current_member);
    sfx->play("menu_navigate");
    portrait.fade_clock = 0.0;
  }
  else if (Input::pressed(keybind->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void DiagnosePanel::draw() {
  Rectangle source = {0, 0, 274, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  if (state == PanelState::READY) {
    drawMemberName();
    drawPortrait();
  }
}

void DiagnosePanel::drawMemberName() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Character *party_member = *current_member;
  std::string name = party_member->name;

  Vector2 position = TextUtils::alignCenter(name.c_str(), {250, 25}, 
                                            *font, -2, 0);
  DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
}

void DiagnosePanel::drawPortrait() {
  Character *party_member = *current_member;
  PartyMemberID id = party_member->member_id;

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  std::string name = party_member->name;
  for (char &letter : name) {
    letter = std::toupper(letter);
  }

  Vector2 position = TextUtils::alignCenter(name.c_str(), {112, 198}, 
                                            *font, -2, 0);

  DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
  portrait.draw(id);
}
