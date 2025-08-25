#include <raylib.h>
#include "enums.h"
#include "base/combat_action.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/knife_cleave.h"


KnifeCleave::KnifeCleave(Mary *user): 
  CombatAction(ActionID::KNIFE_CLEAVE, ActionType::OFFENSE_HP, user, 
               0.20, 0.10, 0.25) 
{
  name = "Knife Cleave";

  hitbox.scale = {28, 8};
  hitbox.offset = {-14 + (14.0f * user->direction), -50};
  user->rectExCorrection(hitbox);
}

void KnifeCleave::windUp() {

}

void KnifeCleave::action() {

}

void KnifeCleave::endLag() {

}

void KnifeCleave::drawDebug() {
  Color color;
  switch (phase) {
    case ActionPhase::WIND_UP: {
      color = YELLOW;
      break;
    }
    case ActionPhase::ACTIVE: {
      color = RED;
      break;
    }
    case ActionPhase::END_LAG: {
      color = BLUE;
      break;
    }
  }

  color.a = 128;
  DrawRectangleRec(hitbox.rect, color);
}
