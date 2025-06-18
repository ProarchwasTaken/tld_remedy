#pragma once
#include <raylib.h>
#include "base/entity.h"


class FieldCamera : public Camera2D {
public:
  FieldCamera();

  void follow(Entity *entity);
};
