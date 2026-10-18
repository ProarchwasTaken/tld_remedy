#include <raylib.h>
#include "enums.h"
#include "scenes/setup.h"
#include "menu/panels/weapon.h"
#include <plog/Log.h>


WeaponPanel::WeaponPanel() {
  id = PanelID::WEAPON;
  atlas = &SetupScene::atlas;
  atlas->use();
  PLOGI << "Weapon Select Panel has been initialized.";
}

WeaponPanel::~WeaponPanel() {
  atlas->release();
}

void WeaponPanel::update() {

}

void WeaponPanel::draw() {
  DrawTextureRec(atlas->sheet, atlas->sprites[0], {16, 16}, WHITE);
}
