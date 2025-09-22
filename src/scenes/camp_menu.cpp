#include <cassert>
#include <enums.h>
#include "data/session.h"
#include "scenes/camp_menu.h"


CampMenuScene::CampMenuScene(Session *session) {
  this->scene_id = SceneID::CAMP_MENU;
  this->session = session;
}

void CampMenuScene::update() {

}

void CampMenuScene::draw() {

}
