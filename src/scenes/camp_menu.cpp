#include <cassert>
#include <cmath>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "utils/text.h"
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include <plog/Log.h>

using std::string;
SpriteAtlas CampMenuScene::atlas("menu", "camp_menu");


CampMenuScene::CampMenuScene(Session *session) {
  PLOGI << "Loading the Camp Menu Scene.";
  this->scene_id = SceneID::CAMP_MENU;
  this->session = session;
  this->keybinds = &Game::settings.menu_keybinds;

  atlas.use();
  main_bar = atlas.getTexturefromSprite(3);

  sfx = &Game::menu_sfx;
  sfx->use();
}

CampMenuScene::~CampMenuScene() {
  atlas.release();
  UnloadTexture(main_bar);
  assert(atlas.users() == 0);

  sfx->release();
  PLOGI << "Unloading the Camp Menu Scene.";
}

void CampMenuScene::update() {
  optionNavigation();
  optionTimer();
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
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    Game::returnToField();
  }
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
  drawTopBar();
  drawBottomBar();
  drawOptions();
}

void CampMenuScene::drawTopBar() {
  Rectangle source = {0 + bar_offset, 0, 426, 40};
  Vector2 position = top_position;
  DrawTextureRec(main_bar, source, position, WHITE);

  drawHeader(position);
  drawTopInfo(position);
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
  Rectangle source = {0 + bar_offset, 0, -426, -40};
  Vector2 position = bottom_position;
  Rectangle dest = {position.x, position.y, 426, 40};
  DrawTexturePro(main_bar, source, dest, {0, 0}, 0, WHITE);

  drawBottomInfo(position);
}

void CampMenuScene::drawBottomInfo(Vector2 position) {
  Rectangle *sprite = &atlas.sprites[5];
  position = Vector2Add(position, {32, 4});
  DrawTextureRec(atlas.sheet, *sprite, position, WHITE);

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  drawSupplyCount(font, txt_size, position);
  drawPlaytime(font, txt_size, position);
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

void CampMenuScene::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  for (int index = 0; index < 5; index++) {
    CampMenuOption option = options[index];

    string name = getOptionName(option);
    Rectangle *sprite;

    Vector2 position = option_position;
    position.y += 16 * index;

    if (option == *selected) {
      position.x += 16 * opt_switch_clock;
      sprite = &atlas.sprites[1];
    }
    else {
      sprite = &atlas.sprites[0];
    }

    DrawTextureRec(atlas.sheet, *sprite, position, WHITE);

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
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

