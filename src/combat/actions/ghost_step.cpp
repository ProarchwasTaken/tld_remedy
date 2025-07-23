#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include <cassert>
#include "combat/actions/ghost_step.h"


GhostStep::GhostStep(Combatant *user, int direction_x): 
  CombatAction(ActionID::GHOST_STEP, ActionType::DEFENSE, user, 
               0.0, 0.20, 0.10)
{
  name = "Ghost Step";

  assert(direction_x != 0);
  velocity = (120 * direction_x) * user->speed_multiplier;
  user->intangible = true;
}

void GhostStep::action() {
  user->position.x += velocity * Game::deltaTime();
  user->rectExCorrection(user->bounding_box, user->hurtbox);

  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->intangible = false;
  }
}

void GhostStep::endLag() {
  float percentage = 1.0 - state_clock;
  float magnitude = velocity * percentage;

  user->position.x += magnitude * Game::deltaTime();
  user->rectExCorrection(user->bounding_box, user->hurtbox);
}
