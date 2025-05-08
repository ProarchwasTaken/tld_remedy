#pragma once
#include <string>
#include <raylib.h>
#include <set>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"


/* Actors are basically the main players of the Field scene.*/
class Actor : public Entity {
public:
  inline static std::set<Actor*> existing_actors;
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
