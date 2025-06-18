#include <cassert>
#include <cstddef>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include <plog/Log.h>


CombatAction::CombatAction(ActionID id, ActionType type, Combatant *user,
                           float wind_up, float action, float end_lag)
{
  this->id = id;
  this->type = type;
  state = ActionState::WIND_UP;

  wind_time = wind_up;
  act_time = action;
  end_time = end_lag;
}

CombatAction::~CombatAction() {
}

void CombatAction::logic() {
  assert(user != NULL);
  assert(user->state == CombatantState::ACTION);

  float *state_time;
  switch (state) {
    case ActionState::WIND_UP: {
      windUp();
      state_time = &wind_time;
      break;
    }
    case ActionState::ACTIVE: {
      action();
      state_time = &act_time;
      break;
    }
    case ActionState::END_LAG: {
      endLag();
      state_time = &end_time;
      break;
    }
  }

  if (*state_time != 0.0 && state_clock < 1.0) {
    state_clock += Game::time() / *state_time;
  }
  else {
    proceed();
  }
}

void CombatAction::proceed() {
  if (state != ActionState::END_LAG) {
    state = static_cast<ActionState>(state + 1);
  }
  else {
    user->state = CombatantState::NEUTRAL;
  }

  state_clock = 0.0;
}
