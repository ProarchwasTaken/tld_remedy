#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "data/session.h"
#include "combat/combatants/player.h"


PlayerCombatant::PlayerCombatant(Player *plr) : 
  Combatant(plr->name, CombatantType::PLAYER, {0, 152}, RIGHT)
{
  life = plr->life;
  max_life = plr->max_life;

  init_morale = plr->init_morale;
  morale = init_morale;
  max_morale = plr->max_morale;

  offense = plr->offense;
  defense = plr->defense;
  intimid = plr->intimid;
  persist = plr->persist;

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {16, 56};
  hurtbox.offset = {-8, -58};

  rectExCorrection(bounding_box, hurtbox);
}

void PlayerCombatant::behavior() {

}

void PlayerCombatant::update() {

}

void PlayerCombatant::draw() {

}
