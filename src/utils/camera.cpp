#include <algorithm>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include <plog/Log.h>
#include "game.h"
#include "base/entity.h"
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
