#include <cassert>
#include <cstddef>
#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "combat/system/camera.h"

Rectangle CombatCamera::area = {
  0, 0, Game::CANVAS_RES.x, Game::CANVAS_RES.y
};


CombatCamera::CombatCamera() { 
  target = {0, 0};
  offset = {Game::CANVAS_RES.x / 2, 0};
  zoom = 1.0;
  rotation = 0;
}

void CombatCamera::update(PartyMember *player) {
  if (player->target != NULL) {
    enemyTargeting(player);
  }
  else {
    follow(player->position.x);
  }

  area.x = target.x - 213;
  area.y = target.y;
}

void CombatCamera::enemyTargeting(PartyMember *player) {
  Combatant *target = player->target;

  Vector2 difference = Vector2Subtract(target->position, player->position);
  Vector2 scale = Vector2Scale(difference, 0.5);
  Vector2 camera_target = Vector2Add(player->position, scale);

  follow(camera_target.x);
}

void CombatCamera::follow(float x) {
  float bounds = 512 - 212;

  if (x >= 0 && x > bounds) {
    x = bounds;
  }
  else if (x < 0 && x < -bounds) {
    x = -bounds;
  }

  Vector2 difference = Vector2Subtract({x, 0}, target);
  float length = Vector2Length(difference);

  if (length > min_length) {
    float speed = std::max(fraction_speed * length, min_speed);
    float delta = Game::deltaTime();

    Vector2 scale = Vector2Scale(difference, speed * delta / length);
    target = Vector2Add(target, scale);
  }
}
