#include "enums.h"
#include "scenes/setup.h"
#include "menu/panels/companion.h"
#include <plog/Log.h>
#include <raylib.h>


CompanionPanel::CompanionPanel() {
  id = PanelID::WEAPON;
  atlas = &SetupScene::atlas;
  atlas->use();

  PLOGI << "Companion Select Panel has been initialized.";
}

CompanionPanel::~CompanionPanel() {
  atlas->release();
}

void CompanionPanel::update() {

}

void CompanionPanel::draw() {
  DrawTextureRec(atlas->sheet, atlas->sprites[1], {16, 16}, WHITE);
}
