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
