#pragma once
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"


/* The Attack CombatAction is a very versatile Action that's designed to
 * be used by any Combatant as long as the right arguments are provided
 * on initialization. It's the most simple way to allow the Combatant to 
 * inflict damage of any kind.*/
class Attack : public CombatAction {
public:
  Attack(Combatant *user, RectEx hitbox);

  void windUp() override {};
  void action() override;
  void endLag() override {};

  void drawDebug() override;
private:
  RectEx hitbox;

  bool attack_connected = false;
  DamageData data;
};
