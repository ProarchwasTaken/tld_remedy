#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "combat/actions/evade.h"
#include <plog/Log.h>


Evade::Evade(PartyMember *user, RectEx hitbox): 
  CombatAction(ActionID::EVADE, ActionType::DEFENSE, user, 
               0.20, 0.40, 0.25) 
{
  name = "Evade";
  this->user = user;

  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;
}

Evade::~Evade() {
  bool user_cancel = !finished && user->state == CombatantState::ACTION;
  if (!user_cancel) {
    user->intangible = false;
    return;
  }

  ActionID new_action_id = user->action->id;

  if (new_action_id != ActionID::GHOST_STEP) {
    user->intangible = false;
  }
}

void Evade::intercept(DamageData &data) {
  bool valid = data.hitbox != NULL && phase == ActionPhase::ACTIVE;
  if (!valid || !CheckCollisionRecs(hitbox.rect, *data.hitbox)) {
    CombatAction::intercept(data);
    return;
  }
  PLOGI << "Conditions have been met to intercept damage function";
  data.b_def = &user->persist;
  data.def_mod += 0.40;

  float damage = Clamp(user->damageCalulation(data), 0, 9999);
  PLOGD << "Result: " << damage;
  if (user->important && state_clock <= 0.25) {
    PLOGI << "Perfect Evasion! Exhaustion depleted!";
    PLOGD << "Timing: " << act_time * state_clock;
    user->depleteInstant();
    Game::sleep(0.5);
  }
  else {
    PLOGD << "Redirection damage towards the combatant's exhaustion.";
    user->increaseExhaustion(damage); 
  }

 
  state_clock = 1.0;
  data.intercepted = true;

  user->intangible = true;
  evaded_attack = true;
  PLOGI << "Interception complete.";
}

void Evade::windUp() {

}

void Evade::action() {

}

void Evade::endLag() {
  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->intangible = false;
  }
}


void Evade::drawDebug() {
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
