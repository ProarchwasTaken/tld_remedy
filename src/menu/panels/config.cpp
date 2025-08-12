#include <cassert>
#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"
#include "menu/panels/config.h"


ConfigPanel::ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas) {
  id = PanelID::CONFIG;
  this->position = position;

  frame = LoadTexture("graphics/menu/config_frame.png");

  assert(menu_atlas != NULL);
  atlas = menu_atlas;
  atlas->use();
}

ConfigPanel::~ConfigPanel() {
  UnloadTexture(frame);
  atlas->release();
}

void ConfigPanel::update() {

}

void ConfigPanel::draw() {
  DrawTextureV(frame, position, WHITE);
}

