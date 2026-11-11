#include <cassert>
#include <raylib.h>
#include "enums.h"
#include "game.h"
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
  gravity = 0.20;
  drag = 20;
  launch(120, -20);

  predictTrajectory(0.25);
  detectOncoming();

  atlas.use();
  sprite = &atlas.sprites[0];
}

DummyProjectile::~DummyProjectile() {
  atlas.release();
}

void DummyProjectile::update() {
  ownerCheck();

  if (wait_clock < 1.0) {
    wait_clock += Game::deltaTime() / wait_time;
    return;
  }

  if (!dying) {
    runPhysics();
    lifeTimer();
    warningProcess();
  }
  else {
    deathTimer();
  }
}

void DummyProjectile::draw() {
  drawSprite(&atlas.sheet);
}
