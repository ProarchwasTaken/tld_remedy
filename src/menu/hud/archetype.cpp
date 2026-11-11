#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"
#include "menu/hud/archetype.h"


SpriteAtlas Archetype::atlas("menu", "archetypes");

Archetype::Archetype(Vector2 position) {
  this->position = position;
  atlas.use();
}

Archetype::~Archetype() {
  atlas.release();
}

void Archetype::setIcon(CompanionID id) {
  int sprite_id = static_cast<int>(id);
  icon = &atlas.sprites[sprite_id];
}

void Archetype::draw() {
  Rectangle source = *icon;
  source.height = source_height;
  DrawTextureRec(atlas.sheet, source, position, WHITE);
}
