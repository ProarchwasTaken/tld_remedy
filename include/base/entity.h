#pragma once
#include <cstddef>
#include <raylib.h>
#include <unordered_set>
#include <vector>
#include <memory>
#include "data/rect_ex.h"
#include "data/animation.h"
#include "enums.h"


/* The Entity abstract class is the root of every game object; Be it 
 * Actors, Combatants, Projectiles, or Particle effects. If it's anything
 * slightly more complex than a structure, then it will most likely
 * derive from this class.*/
class Entity {
public:
  inline static std::unordered_set<int> existing_entities;
  static void clear(std::vector<std::unique_ptr<Entity>> &entity_list);

  Entity();
  virtual ~Entity();

  /* Each and every Entity has an ID that's used to differentiate from one
   * another. Technically this also goes for the entity's type, but the 
   * difference is that the ID is useful for singling out one entity in
   * particular rather than a specific category.*/
  void assignID();

  /* Primarily used to correcting the position of a rect whenever the 
   * entity moves or changes position. For convenience, you could insert 
   * as much arguments as you want, and the function will be called for 
   * each one.*/
  void rectExCorrection(RectEx &rect_ex);

  template<typename... Args>
  void rectExCorrection(Args&... args) {
    (rectExCorrection(args), ...);
  }

  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void drawDebug();

  const int entity_id = -1;
  EntityType entity_type;

  Vector2 position;
  Rectangle *sprite = NULL;
  Animation *animation = NULL;
  RectEx bounding_box;
};
