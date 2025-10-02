#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"
#include "menu/hud/portrait.h"

SpriteAtlas Portrait::atlas("menu", "portraits");


Portrait::Portrait(Vector2 position) {
  this->position = position;
  atlas.use();
}

Portrait::~Portrait() {
  atlas.release();
}

void Portrait::draw(PartyMemberID member_id) {
  int index = static_cast<int>(member_id);
  Rectangle *sprite = &atlas.sprites.at(index);
  DrawTextureRec(atlas.sheet, *sprite, position, WHITE);
}
