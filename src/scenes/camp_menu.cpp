#include <cassert>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include <plog/Log.h>

SpriteAtlas CampMenuScene::atlas("menu", "camp_menu");


CampMenuScene::CampMenuScene(Session *session) {
  PLOGI << "Loading the Camp Menu Scene.";
  this->scene_id = SceneID::CAMP_MENU;
  this->session = session;
  this->keybinds = &Game::settings.menu_keybinds;

  atlas.use();
  main_bar = atlas.getTexturefromSprite(3);
}

CampMenuScene::~CampMenuScene() {
  atlas.release();
  UnloadTexture(main_bar);
  assert(atlas.users() == 0);
  PLOGI << "Unloading the Camp Menu Scene.";
}

void CampMenuScene::update() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    Game::returnToField();
  }

  offsetBars();
}

void CampMenuScene::offsetBars() {
  if (bar_offset > 4) {
    bar_offset = bar_offset - 4;
  }

  bar_offset += offset_speed * Game::deltaTime();
}

void CampMenuScene::draw() {
  drawTopBar();
  drawBottomBar();
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
}
