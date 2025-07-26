#include <cassert>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "system/sprite_atlas.h"
#include "combat/actions/ghost_step.h"


GhostStep::GhostStep(Combatant *user, SpriteAtlas &user_atlas, 
                     int direction_x, GSSpriteSet &sprite_set): 
  CombatAction(ActionID::GHOST_STEP, ActionType::DEFENSE, user, 
               0.0, 0.20, 0.10)
{
  name = "Ghost Step";

  assert(direction_x != 0);
  velocity = (120 * direction_x) * user->speed_multiplier;

  user->intangible = true;
  this->user_atlas = &user_atlas;
  this->sprite_set = &sprite_set;

  if (direction_x == user->direction) {
    user->sprite = &user_atlas.sprites[sprite_set.forward_sprite];
  }
  else {
    user->sprite = &user_atlas.sprites[sprite_set.back_sprite];
  } 
  Combatant::sfx.play("ghost_step");
}

void GhostStep::action() {
  user->position.x += velocity * Game::deltaTime();
  user->rectExCorrection(user->bounding_box, user->hurtbox);

  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->intangible = false;
    user->sprite = &user_atlas->sprites[sprite_set->end_sprite];
  }
}

void GhostStep::endLag() {
  float percentage = 1.0 - state_clock;
  float magnitude = velocity * percentage;

  user->position.x += magnitude * Game::deltaTime();
  user->rectExCorrection(user->bounding_box, user->hurtbox);
}
