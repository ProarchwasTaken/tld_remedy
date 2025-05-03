#pragma once
#include <raylib.h>
#include <unordered_set>
#include "enums.h"


/* An Entity is the root of every game object.*/
class Entity {
  inline static std::unordered_set<int> existing_entities;
public:
  Entity();
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
