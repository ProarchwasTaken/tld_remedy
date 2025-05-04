#pragma once
#include <raylib.h>
#include <unordered_set>
#include "data/rect_ex.h"
#include "enums.h"


/* The Entity abstract class is the root of every game object; Be it 
 * Actors, Combatants, Projectiles, or Particle effects.*/
class Entity {
public:
  inline static std::unordered_set<int> existing_entities;

  Entity();
  virtual ~Entity();

  void assignID();
  void rectExCorrection(RectEx &rect_ex);

  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void drawDebug();

  const int entity_id = -1;
  EntityType entity_type;

  Vector2 position;
  RectEx bounding_box;
};
