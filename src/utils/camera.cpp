#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <map>
#include <raylib.h>
#include <raymath.h>
#include <plog/Log.h>
#include "game.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "combat/combatants/player.h"
#include "utils/camera.h"


Camera2D CameraUtils::setupField() {
  Camera2D camera;
  camera.target = {0, 0};
  camera.offset = {Game::CANVAS_RES.x / 2, Game::CANVAS_RES.y / 2};
  camera.zoom = 1.0;
  camera.rotation = 0;

  return camera;
}

Camera2D CameraUtils::setupCombat() {
  Camera2D camera;
  camera.target = {0, 0};
  camera.offset = {Game::CANVAS_RES.x / 2, 0};
  camera.zoom = 1.0;
  camera.rotation = 0;

  return camera;
}

void CameraUtils::followFieldEntity(Camera2D &camera, Entity *entity) {
  if (entity == NULL) {
    return;
  }

  static float min_length = 1.0;
  static float min_speed = 0.75;
  static float fraction_speed = 0.10;

  Vector2 difference = Vector2Subtract(entity->position, camera.target);
  float length = Vector2Length(difference);

  if (length > min_length) {
    float speed = std::max(fraction_speed * length, min_speed);
    float delta = Game::deltaTime();

    Vector2 scale = Vector2Scale(difference, speed * delta / length);
    camera.target = Vector2Add(camera.target, scale);
  }
}

void CameraUtils::combatCamera(Camera2D &camera, PlayerCombatant *player)
{
  bool enemies_present = Combatant::enemyCount() != 0;
  if (enemies_present) {
    enemyTarget(camera, player);
  }
  else {
    combatFollow(camera, player->position.x);
  }
}

bool maxAlgorithm(const std::pair<int, float> &p1, 
                  const std::pair<int, float> &p2) 
{
  return p1.second < p2.second;
}

void CameraUtils::enemyTarget(Camera2D &camera, PlayerCombatant *player) {
  static Rectangle screen = {
    0, 0, Game::CANVAS_RES.x, Game::CANVAS_RES.y
  };
  screen.x = camera.target.x - 213; 
  screen.y = camera.target.y;

  std::map<int, float> length_table;

  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->combatant_type != CombatantType::ENEMY) {
      continue;
    }

    if (CheckCollisionRecs(screen, combatant->hurtbox.rect)) {
      Vector2 difference = Vector2Subtract(player->position, 
                                           combatant->position);
      float length = Vector2Length(difference);

      length_table.emplace(combatant->entity_id, length);
    }
  }

  int entity_id;
  if (length_table.empty()) {
    combatFollow(camera, player->position.x);
    return;
  }
  else if (length_table.size() == 1) {
    entity_id = length_table.begin()->first;
  }
  else {
    auto furthest = std::max_element(length_table.begin(), 
                                     length_table.end(),
                                     maxAlgorithm);
    entity_id = furthest->first;
  }

  Combatant *enemy = Combatant::getCombatantByID(entity_id);
  assert(enemy != NULL);

  Vector2 difference = Vector2Subtract(enemy->position, player->position);
  Vector2 scale = Vector2Scale(difference, 0.5);
  Vector2 target = Vector2Add(player->position, scale);

  combatFollow(camera, target.x);
}


void CameraUtils::combatFollow(Camera2D &camera, float x) {
  float bounds = 512 - 213;

  if (x >= 0 && x > bounds) {
    x = bounds;
  }
  else if (x < 0 && x < -bounds) {
    x = -bounds;
  }

  static float min_length = 1.0;
  static float min_speed = 0.30;
  static float fraction_speed = 0.05;

  Vector2 difference = Vector2Subtract({x, 0}, camera.target);
  float length = Vector2Length(difference);

  if (length > min_length) {
    float speed = std::max(fraction_speed * length, min_speed);
    float delta = Game::deltaTime();

    Vector2 scale = Vector2Scale(difference, speed * delta / length);
    camera.target = Vector2Add(camera.target, scale);
  }
}
