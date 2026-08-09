#include <cassert>
#include <cmath>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/projectile.h"
#include "data/combat_event.h"
#include <plog/Log.h>

using std::string;


Projectile::Projectile(string name, Combatant *owner, Vector2 position) {
  this->name = name;
  this->position = position;
  entity_type = EntityType::PROJECTILE;

  assert(owner != NULL);
  this->owner = owner;
  alignment = owner->team;

  bool successful = existing_projectiles.emplace(this).second;
  assert(successful);
  PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "]";
}

Projectile::~Projectile() {
  int erased = existing_projectiles.erase(this);
  assert(erased == 1);
  PLOGI << "Removed projectile: '" << name << "'";
}

float Projectile::radians(float degrees) {
  return degrees * (M_PI / 180);
}

float Projectile::degrees(float radians) {
  return radians * (180 / M_PI);
}

void Projectile::launch(float velocity, float angle) {
  this->velocity = velocity;
  this->angle = angle;
  direction = Vector2Rotate({1.0, 0.0}, radians(angle));
}

void Projectile::predictTrajectory(float accuracy) {
  assert(accuracy > 0);
  assert(velocity > 0);
  assert(terminal_velocity <= velocity);

  float mock_velocity = velocity / accuracy;
  float mock_terminal = terminal_velocity / accuracy;
  float mock_gravity = gravity / accuracy;
  float mock_drag = gravity / accuracy;

  Vector2 mock_position = position;
  Vector2 mock_direction = direction;
  float seconds = 0;
  
  do {
    if (gravity != 0) {
      float result = mock_direction.y + mock_gravity;
      mock_direction.y = Clamp(result, -1, 1);
    }

    if (mock_velocity > mock_terminal) {
      mock_velocity -= mock_drag;
    }

    if (mock_velocity < mock_terminal) {
      mock_velocity = mock_terminal;
    }

    Vector2 magnitude;
    magnitude.x = mock_velocity * mock_direction.x;
    magnitude.y = mock_velocity * mock_direction.y;

    mock_position = Vector2Add(mock_position, magnitude);
    trajectory.push_back(mock_position);

    seconds += 1 / accuracy;
  } while (seconds < 120 && inBounds(mock_position));

  PLOGD << "Trajectory Points: " << trajectory.size();
  PLOGD << "Estimated Lifetime: " << seconds << "s";
}

bool Projectile::inBounds(Vector2 position) {
  Rectangle boundaries = {-512, 0, 1024, GROUND_LEVEL};

  if (position.y > GROUND_LEVEL) {
    return false;
  }

  if (position.y < -512) {
    return false;
  }

  if (position.x > 512) {
    return false;
  } 

  if (position.x < -512) {
    return false;
  }

  return true;
}

void Projectile::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hitbox.rect, 1, RED);

  Vector2 offset = Vector2Multiply({velocity, velocity}, direction);
  Vector2 end_pos = Vector2Add({position}, offset);
  DrawLineV(position, end_pos, YELLOW);

  if (trajectory.empty()) {
    return;
  }

  for (Vector2 point : trajectory) {
    DrawCircleV(point, 2, ORANGE);
  }
}
