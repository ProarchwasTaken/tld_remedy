#include <cassert>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/input.h"
#include "scenes/camp_menu.h"
#include <plog/Log.h>


CampMenuScene::CampMenuScene(Session *session) {
  PLOGI << "Loading the Camp Menu Scene.";
  this->scene_id = SceneID::CAMP_MENU;
  this->session = session;
  this->keybinds = &Game::settings.menu_keybinds;
}

CampMenuScene::~CampMenuScene() {
  PLOGI << "Unloading the Camp Menu Scene.";
}

void CampMenuScene::update() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    Game::returnToField();
  }
}

void CampMenuScene::draw() {

}
