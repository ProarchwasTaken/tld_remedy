#include <raylib.h>
#include <cstddef>
#include "enums.h"
#include "data/entity.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "field/entities/save_point.h"
#include <plog/Log.h>

SpriteAtlas SavePoint::atlas("entities", "savepoint_entity");


SavePoint::SavePoint(SavePointData &data) {
  entity_type = EntityType::SAVE_POINT;

  position = data.position;
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -16};
  rectExCorrection(bounding_box);

  atlas.use();
  sprite = &atlas.sprites[0];
  PLOGI << "Entity Created: SavePoint [ID: " << entity_id << "]";
}

SavePoint::~SavePoint() {
  atlas.release();
}

void SavePoint::update() {
  SpriteAnimation::play(animation, &anim_idle, true);
  sprite = &atlas.sprites[*animation->current];
}

void SavePoint::draw() {
  DrawTexturePro(atlas.sheet, *sprite, bounding_box.rect, {0, 0}, 0, 
                 WHITE);
}
