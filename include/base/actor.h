#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"


class Actor : public Entity {
public:
  Actor(std::string name, Vector2 position, enum Direction direction);
  virtual void drawDebug() override;

  std::string name;
  Direction direction;
  RectEx collis_box;
};
