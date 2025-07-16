#include <cassert>
#include <cmath>
#include <vector>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "system/sprite_atlas.h"
#include "data/field_event.h"
#include "data/line.h"
#include "field/system/field_handler.h"
#include "field/system/field_map.h"
#include "field/actors/player.h"
#include "field/actors/enemy.h"
#include <plog/Log.h>

using std::vector;
SpriteAtlas EnemyActor::atlas("actors", "enemy_actor");
int EnemyActor::pursuing_enemy = -1;


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
  correctSightRect();

  atlas.use();
  sprite = getIdleSprite();
}

EnemyActor::~EnemyActor() {
  if (pursuing_enemy == entity_id) {
    pursuing_enemy = -1;
  }

  routine.clear();
  atlas.release();
}

void EnemyActor::correctSightRect() {
  float half = sight_range / 2;
  if (direction == LEFT || direction == RIGHT) {
    sight.scale = {sight_range, 8};
    sight.offset = {-half + (half * direction), -4};
  }
  else {
    sight.scale = {8, sight_range};
    sight.offset = {-4, -half + (half * (direction / 2.0f))};
  }

  rectExCorrection(sight);
}

void EnemyActor::behavior() {

}

void EnemyActor::update() {
  if (awaiting_deletion) {
    FieldHandler::raise<DeleteEntityEvent>(FieldEVT::DELETE_ENTITY, 
                                           this->entity_id);
    plr->setControllable(true);
    return;
  }

  if (plr == NULL) {
    Actor *ptr = Actor::getActor(ActorType::PLAYER);
    plr = static_cast<PlayerActor*>(ptr);
    return;
  }

  if (!pursuing) {
    normalLogic();
  }
  else {
    pursue();
  }

  if (CheckCollisionRecs(collis_box.rect, plr->collis_box.rect)) {
    PLOGI << "Player has collided with enemy.";
    FieldHandler::raise<FieldEvent>(FieldEVT::INIT_COMBAT);
    direction = static_cast<Direction>(plr->direction * -1);
    awaiting_deletion = true;
  }
}

void EnemyActor::normalLogic() {
  if (routine.size() != 1) {
    directionRoutine();
  }
  sprite = getIdleSprite();

  if (pursuing_enemy == -1 && sightCheck()) {
    PLOGI << "Player has entered the line of sight of EnemyActor [ID: " 
    << this->entity_id << "]";
    pursuing = true;
    pursuing_enemy = entity_id;

    plr->setControllable(false);
    Game::sleep(1.0);
  }
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
  correctSightRect();
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

bool EnemyActor::sightCheck() {
  if (!CheckCollisionRecs(sight.rect, plr->collis_box.rect)) {
    return false;
  }

  Rectangle area = GetCollisionRec(sight.rect, plr->collis_box.rect);
  float half_width = area.width / 2;
  float half_height = area.height / 2;

  Vector2 end = {area.x + half_width, area.y + half_height};

  Vector2 start;
  if (direction == LEFT || direction == RIGHT) {
    start = {position.x, end.y};
  }
  else {
    start = {end.x, position.y};
  }

  for (Line line : FieldMap::collision_lines) {
    if (CheckCollisionLines(start, end, line.start, line.end, NULL)) {
      return false;
    }
  }
  
  return true;
}

void EnemyActor::pursue() {
  plr->direction = static_cast<Direction>(direction * -1);
  float magnitude = movement_speed * Game::deltaTime();

  position = Vector2MoveTowards(position, plr->position, magnitude);
  rectExCorrection(collis_box, bounding_box);
}

void EnemyActor::draw() {
  assert(sprite != NULL);
  Rectangle dest = bounding_box.rect;

  float sinY = std::sinf(GetTime() * 2) * 1.5;
  dest.y += sinY;

  DrawTexturePro(atlas.sheet, *sprite, dest, {0, 0}, 0, WHITE);
}

void EnemyActor::drawDebug() {
  Actor::drawDebug();

  Color color = YELLOW;
  color.a = 64;
  DrawRectangleRec(sight.rect, color);
}
