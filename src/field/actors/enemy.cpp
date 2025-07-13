#include <cassert>
#include <cmath>
#include <vector>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"
#include "field/actors/enemy.h"

using std::vector;
SpriteAtlas EnemyActor::atlas("actors", "enemy_actor");


EnemyActor::EnemyActor(Vector2 position, vector<Direction> routine, 
                       float speed):
Actor("Enemy", ActorType::ENEMY, position, *routine.begin())
{
  this->routine = routine;
  this->current_direction = this->routine.begin();
  this->speed = speed;

  bounding_box.scale = {32, 48};
  bounding_box.offset = {-16, -44};
  collis_box.scale = {8, 20};
  collis_box.offset = {-4, -16};
  rectExCorrection(bounding_box, collis_box);

  atlas.use();
  sprite = getIdleSprite();
}

EnemyActor::~EnemyActor() {
  routine.clear();
  atlas.release();
}

void EnemyActor::behavior() {

}

void EnemyActor::update() {
  if (routine.size() != 1) {
    directionRoutine();
  }

  sprite = getIdleSprite();
}

void EnemyActor::directionRoutine() {
  routine_clock += Game::deltaTime() / speed;

  if (routine_clock < 1.0) {
    return;
  }

  current_direction++;

  if (current_direction == routine.end()) {
    current_direction = routine.begin();
  }

  direction = *current_direction;
  routine_clock = 0.0;
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
