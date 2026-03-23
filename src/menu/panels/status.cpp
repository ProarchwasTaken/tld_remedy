#include <cmath>
#include <cassert>
#include <cstddef>
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
  frame = LoadTexture("graphics/menu/frames/status.png");

  options[0] = &session->player;
  options[1] = &session->companion;
  selected = options.begin();

  this->keybinds = &Game::settings.menu_keybinds;
  this->description = description;
  updateDescription();

  menu_atlas = &Game::menu_atlas;
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

void StatusPanel::updateDescription() {
  assert(description != NULL);
  assert(selected != NULL);

  Character *party_member = *selected;
  switch (party_member->member_id) {
    case PartyMemberID::MARY: {
      *description = 
        "A young man who used to work as a nurse\n"
        "for the Grand Hills hospital.";
      break;
    }
    case PartyMemberID::ERWIN: {
      *description = 
        "A middle-aged man who had already gone\n"
        "throught the wringer multiple times before.";
      break;
    }
  }
}

void StatusPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else {
    blink_clock += Game::deltaTime();
    optionNavigation();  
  }

  portrait.update(percentage);
}

void StatusPanel::heightLerp() {
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
    updateDescription();

    sfx->play("menu_navigate");
    blink_clock = 0.0;
    portrait.fade_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected);
    updateDescription();

    sfx->play("menu_navigate");
    blink_clock = 0.0;
    portrait.fade_clock = 0.0;
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void StatusPanel::draw() {
  Rectangle source = {0, 0, 160, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  Character *party_member = *selected;
  if (state == PanelState::READY) {
    drawPartyMemberInfo(party_member);
  }

  portrait.draw(party_member->member_id);
  drawOptions();
}

void StatusPanel::drawPartyMemberInfo(Character *party_member) {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;
  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;

  drawName(med_font, med_size, party_member->name);
  drawTitle(med_font, med_size, party_member->member_id);
  drawLife(med_font, med_size, party_member->life, 
           party_member->max_life);
  drawMorale(med_font, med_size, party_member->init_morale, 
             party_member->max_morale);

  // drawClass(sm_font, sm_size, party_member->member_id);
  // drawCombatLvl(sm_font, sm_size, party_member);

  drawStats(sm_font, sm_size, party_member);
  drawStatus(sm_font, sm_size, party_member);
}

void StatusPanel::drawName(Font *font, int txt_size, const char *name) {
  Vector2 position = Vector2Add(frame_position, {2, 1});
  DrawTextEx(*font, name, position, txt_size, -2, WHITE);
}

void StatusPanel::drawTitle(Font *font, int txt_size, PartyMemberID id) {
  string title;
  Color color = Game::palette[40];

  switch (id) {
    case PartyMemberID::MARY: {
      title = "Martyr";
      color = {3, 3, 3, 255};
      break;
    }
    case PartyMemberID::ERWIN: {
      title = "Lost Vagabond";
      break;
    }
  }

  Vector2 position = Vector2Add(frame_position, {156, 1});
  position = TextUtils::alignRight(title.c_str(), position, *font, -2, 0);

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
    case PartyMemberID::MARY: 
    case PartyMemberID::ERWIN: {
      text = "HUMAN - MID TIER";
      break;
    }
  }

  Vector2 position = Vector2Add(frame_position, {41, 48});
  Color color = Game::palette[51];
  DrawTextEx(*font, text.c_str(), position, txt_size, -3, color);
}

void StatusPanel::drawCombatLvl(Font *font, int txt_size, 
                                Character *member)
{
  float stat_sum = member->offense;
  stat_sum += member->defense;
  stat_sum += member->intimid;
  stat_sum += member->persist;
  stat_sum += member->dexterity;
  stat_sum += member->discipline;

  float level = stat_sum / 6;
  const char *text = TextFormat("%00.02f", level);

  Vector2 position = Vector2Add(frame_position, {70, 60});
  Color color = Game::palette[22];
  DrawTextEx(*font, text, position, txt_size, -3, color); 
}

void StatusPanel::drawStats(Font *font, int txt_size, Character *member) {
  char stat[5];
  std::strcpy(stat, TextFormat("%02i", member->offense));
  Vector2 position = Vector2Add(frame_position, {62, 50});
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->defense));
  position.x += 80;
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->intimid));
  position = Vector2Add(frame_position, {62, 63});
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->persist));
  position.x += 80;
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->dexterity));
  position = Vector2Add(frame_position, {62, 76});
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%02i", member->discipline));
  position.x += 80;
  DrawTextEx(*font, stat, position, txt_size, -3, WHITE);

  std::strcpy(stat, TextFormat("%01.00f%%", member->recovery * 100));
  position = Vector2Add(frame_position, {75, 89});
  position = TextUtils::alignRight(stat, position, *font, -3, 0);
  DrawTextEx(*font, stat, position, txt_size, -3, Game::palette[51]);
  
  std::strcpy(stat, TextFormat("%01.00f%%", member->resilience * 100));
  position = Vector2Add(frame_position, {155, 89});
  position = TextUtils::alignRight(stat, position, *font, -3, 0);
  DrawTextEx(*font, stat, position, txt_size, -3, Game::palette[51]);
}

void StatusPanel::drawStatus(Font *font, int txt_size, Character *member)
{
  Vector2 position = Vector2Add(frame_position, {80, 105});

  if (member->status_count == 0) {
    Color color = Game::palette[14];
    DrawTextEx(*font, "FINE :D", position, txt_size, -3, color);
    return;
  } 

  Color color = Game::palette[32];
  for (int index = 0; index < member->status_limit; index++) {
    StatusID effect = member->status[index];
    if (effect == StatusID::NONE) {
      continue;
    }

    string name = getStatusName(effect);
    assert(name != "");

    DrawTextEx(*font, name.c_str(), position, txt_size, -3, color);
    position.y += 12;
  }
}

string StatusPanel::getStatusName(StatusID id) {
  switch (id) {
    case StatusID::BROKEN_ARM: {
      return "BROKEN ARM";
    }
    case StatusID::CRIPPLED_LEG: {
      return "CRIPPLED LEG";
    }
    case StatusID::MANGLED: {
      return "MANGLED";
    }
    default: {
      return "";
    }
  }
}

void StatusPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Rectangle sprite = camp_atlas->sprites[0];
  if (state != PanelState::READY) {
    sprite.width *= percentage;
  }

  for (int index = 0; index < 2; index++) {
    Character *party_member = options[index];
    PartyMemberID id = party_member->member_id;

    Vector2 position = option_position;
    position.y += 16 * index;

    DrawTextureRec(camp_atlas->sheet, sprite, position, WHITE);

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
