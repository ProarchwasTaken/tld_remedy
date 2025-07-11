#include <cassert>
#include <cmath>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"
#include "field/actors/enemy.h"

SpriteAtlas EnemyActor::atlas("actors", "enemy_actor");


EnemyActor::EnemyActor(Vector2 position, enum Direction direction):
Actor("Enemy", ActorType::ENEMY, position, direction)
{
  bounding_box.scale = {32, 48};
  bounding_box.offset = {-16, -44};
  collis_box.scale = {8, 20};
  collis_box.offset = {-4, -16};
  rectExCorrection(bounding_box, collis_box);

  atlas.use();
  sprite = getIdleSprite();
}

EnemyActor::~EnemyActor() {
  atlas.release();
}

void EnemyActor::behavior() {

}

void EnemyActor::update() {
  sprite = getIdleSprite();
}

Rectangle *EnemyActor::getIdleSprite() {
  switch (direction) {
    case DOWN: {
      return &atlas.sprites[0];
    }
    case RIGHT: {
      return &atlas.sprites[1];
    }
    case UP: {
      return &atlas.sprites[2];
    }
    case LEFT: {
      return &atlas.sprites[3];
    }
  }
}

void EnemyActor::draw() {
  assert(sprite != NULL);
  Rectangle dest = bounding_box.rect;

  float sinY = std::sinf(GetTime() * 2) * 1.5;
  dest.y += sinY;

  DrawTexturePro(atlas.sheet, *sprite, dest, {0, 0}, 0, WHITE);
}
