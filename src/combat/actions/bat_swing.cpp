#include "enums.h"
#include "base/combat_action.h"
#include "utils/animation.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/bat_swing.h"


BatSwing::BatSwing(Mary *user) : 
  CombatAction(ActionID::BAT_SWING, ActionType::OFFENSE_HP, user, 
               0.5, 0.10, 0.5)
{
  this->user = user;
  this->atlas = &Mary::atlas;

  hitbox.scale = {38, 17};
  updateHitboxOffset();
  user->rectExCorrection(hitbox);
}

BatSwing::~BatSwing() {
  user->animation = NULL;
}

void BatSwing::updateHitboxOffset() {
  hitbox.offset = {-19.0f + (26.0f * user->direction), -43};
}

void BatSwing::windUp() {
  freeTurning();

  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[41];
  }
}

void BatSwing::freeTurning() {
  if (user->moving_x == 0) {
    return;
  }

  Direction next = static_cast<Direction>(user->moving_x);
  if (user->direction != next) {
    user->direction = next;
    updateHitboxOffset();
    user->rectExCorrection(hitbox);
  }
}

void BatSwing::action() {
  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[42];
  }
}

void BatSwing::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}

void BatSwing::drawDebug() {
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
