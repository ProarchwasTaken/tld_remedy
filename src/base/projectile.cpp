#include <cassert>
#include <cmath>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/projectile.h"
#include "data/combat_event.h"
#include "combat/system/evt_handler.h"
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

void Projectile::predictTrajectory(float interval) {
  assert(interval > 0);
  assert(velocity > 0);
  assert(terminal_velocity <= velocity);
  PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] " 
    << "calculating Trajectory...";

  float mock_velocity = velocity;
  float mock_terminal = terminal_velocity;

  Vector2 mock_position = position;
  Vector2 mock_direction = direction;

  int count = 0;
  int frames = Game::TARGET_FPS * interval;
  float unit_time = 1 / Game::TARGET_FPS;
  float seconds = 0.0;
  
  do {
    if (gravity != 0) {
      float magnitude = gravity * unit_time;
      mock_direction.y = Clamp(mock_direction.y + magnitude, -1, 1);
    }

    if (mock_velocity > mock_terminal) {
      mock_velocity -= drag * unit_time;
    }

    Vector2 magnitude;
    magnitude.x = mock_velocity * mock_direction.x * unit_time;
    magnitude.y = mock_velocity * mock_direction.y * unit_time;

    mock_position = Vector2Add(mock_position, magnitude);

    if (count == frames) {
      trajectory.push_back(mock_position);
      count = 0;
    }
    else {
      count++;
    }

    seconds += unit_time;
  } while (seconds < 60 && inBounds(mock_position));

  PLOGD << "Trajectory Points: " << trajectory.size();
  PLOGD << "Estimated Lifetime: " << seconds << "s";
  life_time = seconds;
  life_clock = 0.0;
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

void Projectile::runPhysics() {
  if (gravity != 0) {
    float magnitude = gravity * Game::deltaTime();
    direction.y = Clamp(direction.y + magnitude, -1, 1);
  }

  if (velocity > terminal_velocity) {
    float magnitude = drag * Game::deltaTime();
    velocity = velocity - magnitude;
  }

  Vector2 magnitude;
  magnitude.x = velocity * direction.x * Game::deltaTime();
  magnitude.y = velocity * direction.y * Game::deltaTime();

  position = Vector2Add(position, magnitude);
  rectExCorrection(bounding_box, hitbox);

  float radians = std::atan2f(direction.y, direction.x);
  angle = degrees(radians);
}

void Projectile::lifeTimer() {
  assert(life_clock >= 0);
  life_clock += Game::deltaTime() / life_time;
  
  if (life_clock >= 1.0) {
    PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] " 
      << "has reached the end of it's Life";

    onEndLife();
    life_clock = 1.0;
  }
}

void Projectile::onEndLife() {
  CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY, 
                                       entity_id);
  PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] " 
    << "has now been queued for deletion.";
}

void Projectile::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hitbox.rect, 1, RED);

  Vector2 offset = Vector2Multiply({velocity, velocity}, direction);
  Vector2 end_pos = Vector2Add({position}, offset);
  DrawLineV(position, end_pos, YELLOW);

  if (calc_thread.joinable() || trajectory.empty()) {
    return;
  }

  for (Vector2 point : trajectory) {
    DrawCircleV(point, 1, ORANGE);
  }
}
