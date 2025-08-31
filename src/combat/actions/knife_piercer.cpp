#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "base/combat_action.h"
#include "utils/animation.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/knife_piercer.h"


KnifePiercer::KnifePiercer(Mary *user): 
  CombatAction(ActionID::KNIFE_PIERCER, ActionType::OFFENSE_HP, user, 
               0.30, 0.15, 0.50) 
{
  name = "Knife Piercer";

  hitbox.scale = {32, 6};
  hitbox.offset = {-16 + (16.0f * user->direction), -38};
  user->rectExCorrection(hitbox);

  this->atlas = &Mary::atlas;
  user->sprite = &atlas->sprites.at(24);
}

KnifePiercer::~KnifePiercer() {
  user->animation = NULL;
}

void KnifePiercer::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, false);

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[26];
  }
  else {
    user->sprite = &atlas->sprites[*user->animation->current];
  }
}

void KnifePiercer::action() {

}

void KnifePiercer::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}

void KnifePiercer::drawDebug() {
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
