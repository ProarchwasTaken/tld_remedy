#include <memory>
#include <raylib.h>
#include "base/enemy.h"
#include "enums.h"
#include "data/rect_ex.h"
#include "base/combat_action.h"
#include "combat/actions/attack.h"
#include "combat/combatants/enemy/dummy.h"

using std::unique_ptr, std::make_unique;


Dummy::Dummy(Vector2 position, Direction direction) :  
  Enemy("Dummy", EnemyID::DUMMY, position)
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

void Dummy::attack() {
  RectEx hitbox;
  hitbox.scale = {32, 16};
  hitbox.offset = {-16 + (16.0f * direction), -40};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, hitbox);
  performAction(action);
}

void Dummy::update() {
  switch (state) {
    case CombatantState::ACTION: {
      action->logic();
    }
    default: {

    }
  }
}

void Dummy::draw() {

}

void Dummy::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
