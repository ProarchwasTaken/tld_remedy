#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "combat/actions/attack.h"
#include <plog/Log.h>


Attack::Attack(Combatant *user, RectEx hitbox): 
  CombatAction(ActionID::ATTACK, ActionType::OFFENSE_MP, user, 
               0.10, 0.05, 0.10)
{
  name = "Attack";
  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;

  data.base_damage = 4;
  data.damage_type = DamageType::MORALE;
  data.calulation = DamageType::MORALE;

  data.stun_time = 0.20;
  data.stun_type = StunType::NORMAL;

  data.knockback = 3.5;
  data.assailant = user;
}

void Attack::action() {
  if (attack_connected) {
    return;
  }

  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->team == user->team) {
      continue;
    }

    if (CheckCollisionRecs(hitbox.rect, combatant->hurtbox.rect)) {
      combatant->takeDamage(data);
      attack_connected = true;
      break;
    }
  }
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

