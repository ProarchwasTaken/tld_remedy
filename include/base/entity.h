#pragma once
#include <raylib.h>
#include <unordered_set>
#include "enums.h"


/* The Entity abstract class is the root of every game object; Be it 
 * Actors, Combatants, Projectiles, or Particle effects.*/
class Entity {
  inline static std::unordered_set<int> existing_entities;
public:
  Entity();
  ~Entity();

  void assignID();
  void boundRectCorrection();

  virtual void draw() = 0;
  virtual void drawDebug();

  int entity_id;
  EntityType entity_type;
  Vector2 position;

  Rectangle bound;
  Vector2 bound_scale;
  Vector2 bound_offset;
};
