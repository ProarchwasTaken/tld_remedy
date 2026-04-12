#include <cassert>
#include <cmath>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "data/actor.h"
#include "data/field_event.h"
#include "data/line.h"
#include "system/sprite_atlas.h"
#include "scenes/field.h"
#include "field/system/field_handler.h"
#include "field/system/field_map.h"
#include "field/actors/player.h"
#include "field/actors/enemy.h"
#include <plog/Log.h>

SpriteAtlas EnemyActor::atlas("actors", "enemy_actor");
int EnemyActor::pursuing_enemy = -1;


EnemyActor::EnemyActor(EnemyActorData &data):
Actor("Enemy", ActorType::ENEMY, data.position, *data.routine.begin())
{
  object_id = data.object_id;
  troop_id = data.troop_id;

  this->routine = data.routine;
  this->current_direction = this->routine.begin();
  this->speed = data.speed;

  bounding_box.scale = {32, 38};
  bounding_box.offset = {-16, -34};
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

void EnemyActor::update() {
  if (awaiting_deletion) {
    FieldHandler::raise<DeleteEntityEvent>(FieldEVT::DELETE_ENTITY, 
                                           this->entity_id);
    FieldHandler::raise<MarkAsDeadEvent>(FieldEVT::MARK_AS_DEAD,
                                         object_id);
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

  // Oh! It turns out past me had already thought of that.
  if (CheckCollisionRecs(collis_box.rect, plr->collis_box.rect)) {
    PLOGI << "Player has collided with enemy.";
    FieldHandler::raise<InitCombatEvent>(FieldEVT::INIT_COMBAT, troop_id);
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
    emote = &emotes.sprites[0];

    plr->setControllable(false);

    FieldScene::sfx.play("enemy_alert");
    Game::bgm->fade(0.0, 1.0);
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
  if (!CheckCollisionPointRec(plr->position, sight.rect)) {
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
  if (emote != NULL) {
    emote = NULL;
  }

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

  if (emote != NULL) {
    drawEmote();
  }
}

void EnemyActor::drawDebug() {
  Actor::drawDebug();

  Color color = YELLOW;
  color.a = 64;
  DrawRectangleRec(sight.rect, color);
}
