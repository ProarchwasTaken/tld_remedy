#include <cassert>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <map>
#include "game.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "base/enemy.h"
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
  bool enemies_present = Enemy::memberCount() != 0;
  if (enemies_present) {
    enemyTargeting(player);
  }
  else {
    follow(player->position.x);
  }

  area.x = target.x - 213;
  area.y = target.y;
}

bool maxAlgorithm(const std::pair<Combatant*, float> &p1, 
                  const std::pair<Combatant*, float> &p2) 
{
  return p1.second < p2.second;
}

void CombatCamera::enemyTargeting(PartyMember *player) {
  std::map<Combatant*, float> length_table;

  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->team != CombatantTeam::ENEMY) {
      continue;
    }

    if (CheckCollisionRecs(area, combatant->hurtbox.rect)) {
      Vector2 difference = Vector2Subtract(player->position, 
                                           combatant->position);
      float length = Vector2Length(difference);

      length_table.emplace(combatant, length);
    }
  }

  Combatant *enemy = NULL;
  if (length_table.empty()) {
    follow(player->position.x);
    return;
  }
  else if (length_table.size() == 1) {
    enemy = length_table.begin()->first;
  }
  else {
    auto furthest = std::max_element(length_table.begin(), 
                                     length_table.end(),
                                     maxAlgorithm);
    enemy = furthest->first;
  }
  assert(enemy != NULL);

  Vector2 difference = Vector2Subtract(enemy->position, player->position);
  Vector2 scale = Vector2Scale(difference, 0.5);
  Vector2 target = Vector2Add(player->position, scale);

  follow(target.x);
}

void CombatCamera::follow(float x) {
  float bounds = 512 - 213;

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
