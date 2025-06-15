#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"


class DummyEnemy : public Combatant {
public:
  DummyEnemy(Vector2 position, Direction direction);

  void behavior() override {};
  void update() override;
  void draw() override;
};
