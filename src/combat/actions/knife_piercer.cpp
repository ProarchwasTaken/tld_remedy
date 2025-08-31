#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/combat_action.h"
#include "utils/animation.h"
#include "utils/collision.h"
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

  data.damage_type = DamageType::LIFE;
  data.calulation = DamageType::LIFE;

  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;

  data.knockback = 45.0;
  data.hit_stop = 0.2;

  data.assailant = user;

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
  float speed = velocity * user->speed_multiplier;
  float magnitude = speed * Game::deltaTime();

  Direction direction = user->direction;
  if (Collision::checkX(this->user, magnitude, direction)) {
    Collision::snapX(this->user, direction);
    state_clock = 1.0;
  }
  else {
    user->position.x += magnitude * direction;
  }

  user->rectExCorrection(user->bounding_box, user->hurtbox, hitbox);
}

void KnifePiercer::endLag() {
  float percentage = 1.0 - state_clock;
  float speed = (velocity * percentage) * user->speed_multiplier;
  float magnitude = speed * Game::deltaTime();

  Direction direction = user->direction;
  if (Collision::checkX(this->user, magnitude, direction)) {
    Collision::snapX(this->user, direction);
  }
  else {
    user->position.x += magnitude * direction;
  }

  user->rectExCorrection(user->bounding_box, user->hurtbox, hitbox);

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
