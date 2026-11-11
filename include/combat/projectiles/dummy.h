#pragma once
#include <raylib.h>
#include "base/combatant.h"
#include "base/projectile.h"
#include "system/sprite_atlas.h"


class DummyProjectile : public Projectile {
public:
  DummyProjectile(Combatant *owner, Vector2 position);
  ~DummyProjectile();

  void update() override;
  void draw() override;

  static SpriteAtlas atlas;
private:
  float wait_time = 3.0;
  float wait_clock = 0.0;
};
