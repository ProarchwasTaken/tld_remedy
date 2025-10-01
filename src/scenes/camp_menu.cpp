#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "utils/text.h"
#include "utils/math.h"
#include "system/sprite_atlas.h"
#include "menu/panels/config.h"
#include "menu/panels/status.h"
#include "menu/panels/confirm.h"
#include "scenes/camp_menu.h"
#include <plog/Log.h>

using std::string, std::make_unique;
SpriteAtlas CampMenuScene::atlas("menu", "camp_menu");
SpriteAtlas CampMenuScene::menu_atlas("menu", "menu_elements");


CampMenuScene::CampMenuScene(Session *session) {
  PLOGI << "Loading the Camp Menu Scene.";
  this->scene_id = SceneID::CAMP_MENU;
  this->keybinds = &Game::settings.menu_keybinds;
  this->session = session;

  plr_hud.assign(&session->player, &state_clock);
  com_hud.assign(&session->companion, &state_clock);

  atlas.use();
  main_bar = atlas.getTexturefromSprite(3);

  sfx = &Game::menu_sfx;
  sfx->use();
}

CampMenuScene::~CampMenuScene() {
  atlas.release();
  sfx->release();
  UnloadTexture(main_bar);
  PLOGI << "Unloading the Camp Menu Scene.";
}

void CampMenuScene::update() {
  assert(session != NULL);
  if (end_session) {
    Game::loadTitleScreen();
    session = NULL;
    return;
  }

  switch (state) { 
    case OPENING: {
      state_clock += Game::deltaTime() / state_time;
      state_clock = Clamp(state_clock, 0.0, 1.0);

      if (state_clock == 1.0) {
        state = READY;
      }
      break;
    }
    case CLOSING: {
      state_clock -= Game::deltaTime() / state_time;
      state_clock = Clamp(state_clock, 0.0, 1.0);

      if (state_clock == 0.0) {
        Game::returnToField();    
      }
      break;
    }
    case OPENING_PANEL: {
      panel_clock += Game::deltaTime() / panel_time;
      panel_clock = Clamp(panel_clock, 0.0, 1.0);

      if (panel_clock == 1.0) {
        panel_mode = true;
        state = READY;
      }
      break;
    }
    case CLOSING_PANEL: {
      panel_clock -= Game::deltaTime() / panel_time;
      panel_clock = Clamp(panel_clock, 0.0, 1.0);

      if (panel_clock == 0.0) {
        plr_hud.clock = &state_clock;
        plr_hud.reverse = false;

        com_hud.clock = &state_clock;
        com_hud.reverse = false;
        state = READY;
      }

      break;
    }
    case READY: {
      if (!panel_mode) {
        optionNavigation();
        optionTimer();
        break;
      }

      assert(panel != nullptr);
      panel->update();

      if (panel->terminate) {
        panelTermination();
      }
      break;
    }
  }

  offsetBars();
}

void CampMenuScene::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected);
    sfx->play("menu_navigate");
    opt_switch_clock = 0.0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected);
    sfx->play("menu_navigate");
    opt_switch_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = CLOSING;
    sfx->play("menu_cancel");
  }
}

void CampMenuScene::selectOption() {
  switch (*selected) {
    case CampMenuOption::STATUS: {
      panel = make_unique<StatusPanel>(&menu_atlas, keybinds, session,
                                       &description);
      break;
    }
    case CampMenuOption::CONFIG: {
      panel = make_unique<ConfigPanel>(&menu_atlas, keybinds);
      break;
    }
    case CampMenuOption::END_GAME: {
      string message = "Return to the Title Screen?\n"
      "(Unsaved progress will be lost.)";

      panel = make_unique<ConfirmPanel>(&menu_atlas, keybinds, message);
    }
    default: {

    }
  }

  if (panel != nullptr) {
    plr_hud.clock = &panel_clock;
    plr_hud.reverse = true;

    com_hud.clock = &panel_clock;
    com_hud.reverse = true;

    state = OPENING_PANEL;
  }
}

void CampMenuScene::panelTermination() {
  assert(panel != nullptr);

  switch (panel->id) {
    case PanelID::CONFIRM: {
      ConfirmPanel *ptr = static_cast<ConfirmPanel*>(panel.get());
      if (*ptr->selected == ConfirmOption::YES) {
        end_session = true;
        Game::fadeout(1.0);
      }
      break;
    }
    default: {

    }
  }

  panel.reset();
  panel_mode = false;
  state = CLOSING_PANEL;
}

void CampMenuScene::offsetBars() {
  if (bar_offset > 8) {
    bar_offset = bar_offset - 8;
  }

  bar_offset += offset_speed * Game::deltaTime();
}

void CampMenuScene::optionTimer() {
  if (opt_switch_clock == 1.0) {
    return;
  }

  opt_switch_clock += Game::deltaTime() / opt_switch_time;
  opt_switch_clock = Clamp(opt_switch_clock, 0.0, 1.0);
} 

void CampMenuScene::draw() {
  if (session == NULL) {
    return;
  }

  drawTopBar();
  drawBottomBar();

  drawOptions();
  plr_hud.draw();
  com_hud.draw();

  if (panel_mode) {
    panel->draw();
  }
}

void CampMenuScene::drawTopBar() {
  Vector2 position;
  if (state != READY) {
    float percentage = Clamp(state_clock / 0.5, 0.0, 1.0);

    position.x = top_position.x;
    position.y = Math::smoothstep(-40, 0, percentage);
  }
  else {
    position = top_position; 
  }

  Rectangle source = {0 + bar_offset, 0, 426, 40};
  DrawTextureRec(main_bar, source, position, WHITE);

  drawHeader(position);

  if (state == READY) {
    drawTopInfo(position);
  }
}

void CampMenuScene::drawHeader(Vector2 position) {
  Rectangle *sprite = &atlas.sprites[4];
  position = Vector2Add(position, {9, 16});
  DrawTextureRec(atlas.sheet, *sprite, position, WHITE);
}

void CampMenuScene::drawTopInfo(Vector2 position) {
  Rectangle *sprite = &atlas.sprites[6];
  position = Vector2Add(position, {176, 22});
  DrawTextureRec(atlas.sheet, *sprite, position, WHITE);

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  position = Vector2Add(position, {11, 1});

  DrawTextEx(*font, "The Outside - Cathedral", position, txt_size, -2, 
             WHITE);
}

void CampMenuScene::drawBottomBar() {
  Vector2 position;
  if (state != READY) {
    float percentage = Clamp(state_clock / 0.5, 0.0, 1.0);

    position.x = bottom_position.x;
    position.y = Math::smoothstep(240, bottom_position.y, percentage);
  }
  else {
    position = bottom_position;
  }

  Rectangle source = {0 + bar_offset, 0, -426, -40};
  Rectangle dest = {position.x, position.y, 426, 40};
  DrawTexturePro(main_bar, source, dest, {0, 0}, 0, WHITE);

  drawBottomInfo(position);
}

void CampMenuScene::drawBottomInfo(Vector2 position) {
  Rectangle *sprite = &atlas.sprites[5];
  position = Vector2Add(position, {32, 4});
  DrawTextureRec(atlas.sheet, *sprite, position, WHITE);

  if (state == READY) {
    Font *font = &Game::med_font;
    int txt_size = font->baseSize;

    drawSupplyCount(font, txt_size, position);
    drawPlaytime(font, txt_size, position);
    drawDescription(font, txt_size, position);
  }
}

void CampMenuScene::drawSupplyCount(Font *font, int txt_size, 
                                    Vector2 position)
{
  const char *text = TextFormat("%03i", session->supplies);
  position.x = position.x + 104;
  position = TextUtils::alignRight(text, position, *font, -2, 0);

  DrawTextEx(*font, text, position, txt_size, -2, WHITE);
}

void CampMenuScene::drawPlaytime(Font *font, int txt_size, 
                                 Vector2 position)
{
  long playtime = std::floor(Game::playtime());
  int seconds = playtime % 60;
  int minutes = (playtime / 60) % 60;
  int hours = playtime / 3600;

  const char *text = TextFormat("%02i:%02i:%02i", hours, minutes, 
                                seconds);
  position = Vector2Add(position, {104, 16});
  position = TextUtils::alignRight(text, position, *font, -2, 0);

  DrawTextEx(*font, text, position, txt_size, -2, WHITE);
}

void CampMenuScene::drawDescription(Font *font, int txt_size, 
                                    Vector2 position)
{
  if (!panel_mode) {
    description = getDescription(*selected);
  }

  position.x += 136;
  DrawTextEx(*font, description.c_str(), position, txt_size, -2, 
             desc_color);
}

string CampMenuScene::getDescription(CampMenuOption option) {
  switch (option) {
    case CampMenuOption::ITEMS: {
      return 
      "Use items the party has obtained over the\n"
      "course of this session.";
    }
    case CampMenuOption::TECHS: {
      return 
      "View information about the Techniques a\n"
      "party member can perform in Combat.";
    }
    case CampMenuOption::STATUS: {
      return 
      "View a party member's stats and overall\n"
      "condition.";
    }
    case CampMenuOption::CONFIG: {
      return 
      "Configure the game's settings to your\n"
      "personal liking.";
    }
    case CampMenuOption::END_GAME: {
      return 
      "End your current session and return to\n"
      "the title screen.";
    }
  }
}

void CampMenuScene::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 base_position;
  Color base_tint = WHITE;
  baseOptionLerp(base_position, base_tint);

  for (int index = 0; index < 5; index++) {
    CampMenuOption option = options[index];

    string name = getOptionName(option);
    Rectangle *sprite;
    Color tint = base_tint;

    Vector2 position = base_position;
    position.y += 16 * index;

    if (option == *selected) {
      selectedOptionLerp(position);
      sprite = &atlas.sprites[1];
    }
    else {
      unselectedOptionLerp(position.x, tint.a);
      sprite = &atlas.sprites[0];
    }

    DrawTextureRec(atlas.sheet, *sprite, position, tint);

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, name.c_str(), position, txt_size, -2, tint);
  }
}

void CampMenuScene::baseOptionLerp(Vector2 &base_position, 
                                   Color &base_tint) 
{
  if (state == OPENING || state == CLOSING) {
    float percentage = Clamp((-0.50 + state_clock) / 0.4, 0.0, 1.0);

    base_position.x = Math::smoothstep(-36, option_position.x, 
                                       percentage);
    base_position.y = option_position.y;

    base_tint.a = Lerp(0, 255, percentage);
  }
  else {
    base_position = option_position;
  }
}

void CampMenuScene::selectedOptionLerp(Vector2 &position) {
  position.x += 16 * opt_switch_clock;

  if (state == OPENING_PANEL || state == CLOSING_PANEL) {
    float start_y = position.y;
    float end_y = option_position.y;
    position.y = Lerp(start_y, end_y, panel_clock);
  }
  else if (state == READY && panel_mode) {
    position.y = option_position.y;
  }
}

void CampMenuScene::unselectedOptionLerp(float &x, unsigned char &alpha) {
  if (state == OPENING_PANEL || state == CLOSING_PANEL) {
    float percentage = Clamp(panel_clock / 0.5, 0.0, 1.0);

    x = Lerp(option_position.x, -18, percentage);
    alpha = Lerp(255, 0, percentage);
  }
  else if (state == READY && panel_mode) {
    alpha = 0;
  }
}

string CampMenuScene::getOptionName(CampMenuOption option) {
  switch (option) {
    case CampMenuOption::ITEMS: {
      return "ITEMS";
    }
    case CampMenuOption::TECHS: {
      return "TECHS";
    }
    case CampMenuOption::STATUS: {
      return "STATUS";
    }
    case CampMenuOption::CONFIG: {
      return "CONFIG";
    }
    case CampMenuOption::END_GAME: {
      return "END GAME";
    }
  }
}

