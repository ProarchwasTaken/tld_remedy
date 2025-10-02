#include <cmath>
#include <cassert>
#include <cstring>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "utils/text.h"
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

  if (state == PanelState::READY) {
    drawPartyMemberInfo();
  }

  drawOptions();
}

void StatusPanel::drawPartyMemberInfo() {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;
  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;
  Character *party_member = *selected;

  drawName(med_font, med_size, party_member->name);
  drawTitle(med_font, med_size, party_member->member_id);
  drawLife(med_font, med_size, party_member->life, 
           party_member->max_life);
  drawMorale(med_font, med_size, party_member->init_morale, 
             party_member->max_morale);

  drawClass(sm_font, sm_size, party_member->member_id);
  drawStyle(sm_font, sm_size, party_member->member_id);

  drawStats(sm_font, sm_size, party_member);
}

void StatusPanel::drawName(Font *font, int txt_size, const char *name) {
  Vector2 position = Vector2Add(frame_position, {2, 1});
  DrawTextEx(*font, name, position, txt_size, -2, WHITE);
}

void StatusPanel::drawTitle(Font *font, int txt_size, PartyMemberID id) {
  string title;
  switch (id) {
    case PartyMemberID::MARY: {
      title = "Former Nurse";
      break;
    }
    case PartyMemberID::ERWIN: {
      title = "Lost Vagabond";
      break;
    }
  }

  Vector2 position = Vector2Add(frame_position, {156, 1});
  position = TextUtils::alignRight(title.c_str(), position, *font, -2, 0);
  Color color = Game::palette[40];

  DrawTextEx(*font, title.c_str(), position, txt_size, -2, color);
}

void StatusPanel::drawLife(Font *font, int txt_size, float value, 
                           float max) 
{
  const char *life = TextFormat("%02.00f / %02.00f", value, max);
  Color color;

  if (value < max * 0.30) {
    color = Game::palette[33];
  }
  else {
    color = WHITE;
  }

  Vector2 position = Vector2Add(frame_position, {154, 16});
  position = TextUtils::alignRight(life, position, *font, -2, 0);

  DrawTextEx(*font, life, position, txt_size, -2, color);
}

void StatusPanel::drawMorale(Font *font, int txt_size, float initial,
                             float max) 
{
  const char *morale = TextFormat("%02.00f / %02.00f", initial, max);
  Color color;

  if (max != 0.0) {
    color = WHITE;
  }
  else {
    color = Game::palette[2];
  }

  Vector2 position = Vector2Add(frame_position, {154, 31});
  position = TextUtils::alignRight(morale, position, *font, -2, 0);

  DrawTextEx(*font, morale, position, txt_size, -2, color);
}

void StatusPanel::drawClass(Font *font, int txt_size, PartyMemberID id) {
  string text;
  switch (id) {
    case PartyMemberID::MARY: {
      text = "HUMAN - HEIR";
      break;
    }
    case PartyMemberID::ERWIN: {
      text = "HUMAN - MAVERICK";
      break;
    }
  }

  Vector2 position = Vector2Add(frame_position, {41, 48});
  Color color = Game::palette[51];
  DrawTextEx(*font, text.c_str(), position, txt_size, -3, color);
}

void StatusPanel::drawStyle(Font *font, int txt_size, PartyMemberID id) {
  string text;
  switch (id) {
    case PartyMemberID::MARY:
    case PartyMemberID::ERWIN: {
      text = "W.TECH - knifekind";
      break;
    }
  }

  Vector2 position = Vector2Add(frame_position, {41, 60});
  Color color = Game::palette[42];
  DrawTextEx(*font, text.c_str(), position, txt_size, -3, color);
}

void StatusPanel::drawStats(Font *font, int txt_size, Character *member) {
  char stat[3];
  std::strcpy(stat, TextFormat("%02i", member->offense));
  Vector2 position = Vector2Add(frame_position, {62, 76});
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->defense));
  position.x += 80;
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->intimid));
  position = Vector2Add(frame_position, {62, 89});
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->persist));
  position.x += 80;
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);
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
