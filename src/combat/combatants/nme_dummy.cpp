#include <memory>
#include <raylib.h>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "combat/actions/attack.h"
#include "combat/combatants/nme_dummy.h"

using std::unique_ptr, std::make_unique;


DummyEnemy::DummyEnemy(Vector2 position, Direction direction) : 
  Combatant("Dummy", CombatantTeam::ENEMY, position, direction)
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

void DummyEnemy::attack() {
  RectEx hitbox;
  hitbox.scale = {32, 16};
  hitbox.offset = {-16 + (16.0f * direction), -40};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, hitbox);
  performAction(action);
}

void DummyEnemy::update() {
  switch (state) {
    case CombatantState::ACTION: {
      action->logic();
    }
    default: {

    }
  }
}

void DummyEnemy::draw() {

}

void DummyEnemy::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
