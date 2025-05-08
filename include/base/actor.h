#pragma once
#include <string>
#include <raylib.h>
#include <set>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"


/* Actors are the main focus of the FieldScene, as do Actors in a 
 * threatre. Typically they are used to portray characters, example 
 * being: the game's player character, and it's npcs. Functionally, they
 * are expected to be much more complex compared to every other entity
 * in the scene.*/
class Actor : public Entity {
public:
  inline static std::set<Actor*> existing_actors;
  /* Like entities, the game keeps track of what actors currently exist
   * by default. For specific situations this functions helps in finding
   * and returning an actor of a specific type. Especially useful when
   * you know that there's only one actor that exists with that type.*/
  static Actor* getActor(enum ActorType type);

  Actor(std::string name, enum ActorType actor_type, Vector2 position, 
        enum Direction direction);
  ~Actor();

  virtual void behavior() = 0;
  virtual void drawDebug() override;

  std::string name;
  ActorType actor_type;
  Direction direction;
  RectEx collis_box;
};
