#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "base/entity.h"
#include "field/system/camera.h"


FieldCamera::FieldCamera() {
  target = {0, 0};
  offset = {Game::CANVAS_RES.x / 2, Game::CANVAS_RES.y / 2};
  zoom = 1.0;
  rotation = 0;
}

void FieldCamera::follow(Entity *entity) {
  if (entity == NULL) {
    return;
  }

  static float min_length = 1.0;
  static float min_speed = 0.75;
  static float fraction_speed = 0.10;

  Vector2 difference = Vector2Subtract(entity->position, target);
  float length = Vector2Length(difference);

  if (length > min_length) {
    float speed = std::max(fraction_speed * length, min_speed);
    float delta = Game::deltaTime();

    Vector2 scale = Vector2Scale(difference, speed * delta / length);
    target = Vector2Add(target, scale);
  }
}
