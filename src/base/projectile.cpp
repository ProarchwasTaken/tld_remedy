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
#include "data/combatant_event.h"
#include "combat/system/evt_handler.h"
#include "combat/system/cbt_handler.h"
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

  if (!oncoming_collisions.empty()) {
    oncoming_collisions.clear();
  }

  if (!trajectory.empty()) {
    trajectory.clear();
  }

  PLOGI << "Removed projectile: '" << name << "'";
}

float Projectile::radians(float degrees) {
  return degrees * (M_PI / 180);
}

float Projectile::degrees(float radians) {
  return radians * (180 / M_PI);
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

void Projectile::launch(float velocity, float angle) {
  this->velocity = velocity;
  this->angle = angle;
  direction = Vector2Rotate({1.0, 0.0}, radians(angle));
}

void Projectile::predictTrajectory(float interval) 
{
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
    seconds += unit_time;

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
      TrajectPoint point = {
        mock_position, 
        mock_direction, 
        mock_velocity, 
        seconds
      };

      trajectory.push_back(point);
      count = 0;
    }
    else {
      count++;
    }
  } while (seconds < max_life_time && inBounds(mock_position));

  PLOGD << "Trajectory Points: " << trajectory.size();
  PLOGD << "Estimated Lifetime: " << seconds << "s";
  life_time = seconds;
  life_clock = 0.0;
}

void Projectile::detectOncoming() {
  PLOGI << "Searching for oncoming collisions in trajectory..";
  if (trajectory.empty()) {
    return;
  }

  PLOGD << "Max Collisions: " << max_collisions;
  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->state == DEAD) {
      continue;
    }

    if (combatant->team == alignment) {
      continue;
    }

    int hit = inTrajectory(combatant);
    if (hit == -1) {
      continue;
    }

    PLOGI << "Detected oncoming collision with Combatant: '" 
      << combatant->name << "' [ID: " << combatant->entity_id << "]";
    TrajectPoint *point = &trajectory.at(hit);
    Rectangle rect = hitbox.rect;
    rect.x = point->position.x + hitbox.offset.x;
    rect.y = point->position.y + hitbox.offset.y;
    
    Rectangle intersect = GetCollisionRec(combatant->hurtbox.rect, rect);
    oncoming_collisions.push_back({combatant, point, intersect});

    if (oncoming_collisions.size() == max_collisions) {
      break;
    }
  }

  PLOGD << "Total Collisions: " << oncoming_collisions.size();
}

int Projectile::inTrajectory(Combatant *combatant) {
  int hit = -1;
  Rectangle *hurtbox = &combatant->hurtbox.rect;

  for (int x = 0; x < trajectory.size(); x++) {
    TrajectPoint *point = &trajectory.at(x);

    if (!inBounds(point->position)) {
      continue;
    }

    Rectangle rect = hitbox.rect;
    rect.x = point->position.x + hitbox.offset.x;
    rect.y = point->position.y + hitbox.offset.y;

    if (CheckCollisionRecs(rect, *hurtbox)) {
      hit = x;
      break;
    }
  }

  return hit;
}

void Projectile::ownerCheck() {
  if (owner == NULL) {
    return;
  }

  if (owner->state == DEAD) {
    PLOGD << "Detected that the projectile's owner is dead.";
    owner = NULL;
  }
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

void Projectile::warningProcess() {
  if (oncoming_collisions.empty()) {
    return;
  }

  float elapsed_time = life_time * life_clock;
  int warned = 0;
  for (auto &collision : oncoming_collisions) {
    if (collision.warned) {
      warned++;
      continue;
    }

    float until_collision = collision.point->seconds - elapsed_time;
    if (until_collision <= warning_time) {
      Combatant *combatant = collision.target;
      PLOGI << "Sending warning to COMBATANT: '" << combatant->name <<
      "' [ID: " << combatant->entity_id << "]";

      CombatantHandler::queue<ProjWarningCBT>(this, 
                                              CombatantEVT::PROJ_WARNING,
                                              combatant, 
                                              *collision.point,
                                              collision.intersect,
                                              until_collision
                                              );
      collision.warned = true;
      warned++;
    }
  }

  if (warned == oncoming_collisions.size()) {
    PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] all"
      << " combatants in trajectory have been warned.";
    oncoming_collisions.clear();
  }
}

void Projectile::lifeTimer() {
  assert(life_clock >= 0);
  life_clock += Game::deltaTime() / life_time;
  
  if (life_clock >= 1.0) {
    onEndLife();
    life_clock = 1.0;
  }
}

void Projectile::onEndLife() {
  PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] " 
    << "has reached the end of it's Life";
  dying = true;
}

void Projectile::deathTimer() {
  death_clock += Game::deltaTime() / death_time;

  if (death_clock >= 1.0) {
    CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY, 
                                         entity_id);
    PLOGI << "PROJECTILE: '" << name << "' [ID: " << entity_id << "] " 
      << "has now been queued for deletion.";
    death_clock = 1.0;
  }

}

void Projectile::drawSprite(Texture *sheet) {
  assert(sprite != NULL);
  Rectangle dest = bounding_box.rect;
  dest.x -= bounding_box.offset.x;
  dest.y -= bounding_box.offset.y;

  Color color = tint;
  if (dying) {
    applyFlicker(death_clock, color);
  }

  DrawTexturePro(*sheet, *sprite, dest, {16, 16}, angle, color);
}

void Projectile::applyFlicker(float x, Color &tint) {
  float sin_a = std::sinf(x * 100);
  sin_a = (sin_a / 2) + 0.5;
  tint.a = 255 * sin_a;
}

void Projectile::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hitbox.rect, 1, RED);

  Vector2 offset = Vector2Multiply({velocity, velocity}, direction);
  Vector2 end_pos = Vector2Add(position, offset);
  DrawLineV(position, end_pos, YELLOW);

  if (!trajectory.empty()) {
    for (TrajectPoint point : trajectory) {
      DrawCircleV(point.position, 1, ORANGE);
    }
  }

  if (!oncoming_collisions.empty()) {
    drawOncoming();
  }
}

void Projectile::drawOncoming() {
  Color intersect_color = RED;
  intersect_color.a = 128;

  for (auto &collision : oncoming_collisions) {
    Vector2 p_position = collision.point->position;
    Vector2 p_direction = collision.point->direction;
    float p_velocity = collision.point->velocity;
    
    Rectangle rect = hitbox.rect;
    rect.x = p_position.x + hitbox.offset.x;
    rect.y = p_position.y + hitbox.offset.y;

    DrawRectangleLinesEx(rect, 1, BLUE); 
    DrawRectangleRec(collision.intersect, intersect_color);

    Vector2 offset = Vector2Multiply({p_velocity, p_velocity}, 
                                     p_direction);
    Vector2 end_pos = Vector2Add(p_position, offset);
    DrawLineV(p_position, end_pos, YELLOW);
  }
}
