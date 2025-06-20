#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "combat/actions/attack.h"
#include <plog/Log.h>


Attack::Attack(Combatant *user, RectEx hitbox): 
  CombatAction(ActionID::ATTACK, ActionType::OFFENSE_MP, user, 
               0.10, 0.05, 0.10)
{
  name = "Attack";
  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;
}

void Attack::drawDebug() {
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

