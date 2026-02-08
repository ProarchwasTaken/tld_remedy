#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "scenes/rest_menu.h"

using std::string;


RestMenuScene::RestMenuScene(Session *session) {
  scene_id = SceneID::REST_MENU;
  this->session = session;

  loadBackground(session->location);
  camera.target = {0, 0};
  camera.offset = {213, 120};
  camera.rotation = 0;
  camera.zoom = 1.0;
}

RestMenuScene::~RestMenuScene() {
  UnloadTexture(background);
}

void RestMenuScene::loadBackground(string location) {
  if (location == "Cathedral" || location == "Debug") {
    background = LoadTexture("graphics/menu/backgrounds/cathedral.png");
    bg_color = Game::palette[40];
  }
}

void RestMenuScene::update() {

}

void RestMenuScene::draw() {
  BeginMode2D(camera);
  {
    DrawTextureV(background, {-250, -150}, bg_color);
  }
  EndMode2D();
}
