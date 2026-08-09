#include <cassert>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/projectile.h"
#include "system/sprite_atlas.h"
#include "combat/projectiles/dummy.h"

SpriteAtlas DummyProjectile::atlas("projectiles", "dummy_projectile");


DummyProjectile::DummyProjectile(Combatant *owner, Vector2 position) : 
  Projectile("Dummy", owner, position)
{
  id = ProjectileID::DUMMY;
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -16};

  hitbox.scale = {16, 16};
  hitbox.offset = {-8, -8};
  rectExCorrection(bounding_box, hitbox);

  terminal_velocity = 40;
  gravity = 0.10;
  drag = 5;
  launch(120, -20);

  predictTrajectory(2);

  atlas.use();
  sprite = &atlas.sprites[0];
}

DummyProjectile::~DummyProjectile() {
  atlas.release();
}

void DummyProjectile::update() {
}

void DummyProjectile::draw() {
  assert(sprite != NULL);
  Rectangle dest = bounding_box.rect;
  dest.x -= bounding_box.offset.x;
  dest.y -= bounding_box.offset.y;
  DrawTexturePro(atlas.sheet, *sprite, dest, {16, 16}, angle, WHITE);
}
