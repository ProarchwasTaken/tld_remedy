#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "scenes/rest_menu.h"

using std::string;


RestMenuScene::RestMenuScene(Session *session) {
  assert(session != NULL);
  scene_id = SceneID::REST_MENU;
  this->session = session;

  loadBackground(session->location);
  camera.target = {0, 0};
  camera.offset = {213, 120};
  camera.rotation = 0;
  camera.zoom = 1.0;

  atlas.use();
  setupCharacters();
}

RestMenuScene::~RestMenuScene() {
  UnloadTexture(background);
  atlas.release();
}

void RestMenuScene::loadBackground(string location) {
  if (location == "Cathedral" || location == "Debug") {
    background = LoadTexture("graphics/menu/backgrounds/cathedral.png");
    bg_color = Game::palette[40];
    char_color = Game::palette[42];
  }
}

void RestMenuScene::setupCharacters() {
  plr_sprite = &atlas.sprites[1];
  plr_position = {59, -21};

  switch (session->companion.companion_id) {
    case CompanionID::ERWIN: {
      com_sprite = &atlas.sprites[2];
      com_position = {4, -21};
    }
  }
}

void RestMenuScene::update() {

}

void RestMenuScene::draw() {
  BeginMode2D(camera);
  {
    DrawTextureV(background, {-250, -150}, bg_color);
    DrawTextureRec(atlas.sheet, *plr_sprite, plr_position, char_color);
    DrawTextureRec(atlas.sheet, *com_sprite, com_position, char_color);
  }
  EndMode2D();
}
