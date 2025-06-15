#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "combat/combatants/nme_dummy.h"


DummyEnemy::DummyEnemy(Vector2 position, Direction direction) : 
  Combatant("Dummy", CombatantType::ENEMY, position, direction)
{
  max_life = 50;
  life = max_life;

  offense = 0;
  defense = 8;
  intimid = 8;
  persist = 4;

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {16, 56};
  hurtbox.offset = {-8, -58};

  rectExCorrection(bounding_box, hurtbox);
}

void DummyEnemy::update() {

}

void DummyEnemy::draw() {

}
