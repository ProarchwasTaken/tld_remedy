#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "combat/actions/attack.h"


Attack::Attack(Combatant *user): 
  CombatAction(ActionID::ATTACK, ActionType::OFFENSE_MP, user, 
               0.25, 0.5, 0.25)
{
  name = "Attack";
}

