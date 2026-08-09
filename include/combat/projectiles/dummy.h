#pragma once
#include <raylib.h>
#include "base/combatant.h"
#include "base/projectile.h"


class DummyProjectile : public Projectile {
public:
  DummyProjectile(Combatant *owner, Vector2 position);
  void update() override;
  void draw() override {};
};
