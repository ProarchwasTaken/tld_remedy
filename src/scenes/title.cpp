#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "scenes/title.h"

TitleScene::TitleScene() {
  scene_id = SceneID::TITLE;
}

TitleScene::~TitleScene() {

}

void TitleScene::update() {

}

void TitleScene::draw() {
  Font *font = &Game::med_font;
  int txt_size = Game::med_font.baseSize;

  DrawTextEx(*font, "THT III: Remedy", {0, 0}, txt_size, -2, 
             Game::palette[14]);
}
