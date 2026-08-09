#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/projectile.h"
#include "combat/projectiles/dummy.h"


DummyProjectile::DummyProjectile(Combatant *owner, Vector2 position) : 
  Projectile("Dummy", owner, position)
{
  id = ProjectileID::DUMMY;
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -16};

  hitbox.scale = {16, 16};
  hitbox.offset = {-8, -8};
  rectExCorrection(bounding_box, hitbox);

  launch(20, 45);
}

void DummyProjectile::update() {

}
