#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/projectile.h"
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

void Projectile::launch(float velocity, float angle) {
  this->velocity = velocity;
  direction = Vector2Rotate({1.0, 0.0}, angle);
}

void Projectile::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hitbox.rect, 1, RED);
}
