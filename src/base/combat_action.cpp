#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/damage.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include <plog/Log.h>


CombatAction::CombatAction(ActionID id, ActionType type, Combatant *user,
                           float wind_up, float action, float end_lag)
{
  this->id = id;
  this->type = type;
  this->user = user;
  phase = ActionPhase::WIND_UP;

  wind_time = wind_up;
  act_time = action;
  end_time = end_lag;
}

CombatAction::~CombatAction() {
}

void CombatAction::intercept(DamageData &data) {
  if (phase == ActionPhase::END_LAG) {
    PLOGI << "User had taken damage while Action is in End Lag phase.";
    data.stun_type = StunType::STAGGER;
    data.calulation = DamageType::LIFE;
    data.damage_type = DamageType::LIFE;

    Combatant::sfx.play("damage_stagger");
    PLOGD << "Forcing Life damage calulation, and Stagger";
  }
}

void CombatAction::logic() {
  assert(user != NULL);
  assert(user->state == CombatantState::ACTION);

  float *state_time;
  switch (phase) {
    case ActionPhase::WIND_UP: {
      windUp();
      state_time = &wind_time;
      break;
    }
    case ActionPhase::ACTIVE: {
      action();
      state_time = &act_time;
      break;
    }
    case ActionPhase::END_LAG: {
      endLag();
      state_time = &end_time;
      break;
    }
  }

  if (*state_time != 0.0 && state_clock < 1.0) {
    state_clock += Game::deltaTime() / *state_time;
    state_clock = Clamp(state_clock, 0.0, 1.0);
  }
  else {
    proceed();
  }
}

void CombatAction::proceed() {
  if (phase != ActionPhase::END_LAG) {
    PLOGD << "ACTION: '" << name << "' owned by COMBATANT: '" << 
      user->name << "' is proceeding to phase: " << phase;
    phase = static_cast<ActionPhase>(phase + 1);
  }
  else {
    PLOGI << "COMBATANT: '" << user->name << "' [ID: " << user->entity_id
      << "] has finished performing ACTION: '" << name << "'";
    user->state = CombatantState::NEUTRAL;
    finished = true;
  }

  state_clock = 0.0;
}
