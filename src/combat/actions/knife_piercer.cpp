#include <cassert>
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

  data.knockback = 110;
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
  movement();
  hitRegistration();
}

void KnifePiercer::movement(float percentage) {
  float speed = velocity * user->speed_multiplier;
  float magnitude = (speed * percentage) * Game::deltaTime();

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

void KnifePiercer::hitRegistration() {
  assert(phase == ActionPhase::ACTIVE);

  for (Combatant *combatant : Combatant::existing_combatants) {
    if (combatant->intangible) {
      continue;
    }

    if (combatant->team == user->team) {
      continue;
    }

    if (combatant->state == CombatantState::DEAD) {
      continue;
    }

    bool already_hit = hits.find(combatant) != hits.end();
    if (already_hit) {
      continue;
    }


    Rectangle *hurtbox = &combatant->hurtbox.rect;
    if (CheckCollisionRecs(hitbox.rect, *hurtbox)) {
      data.hitbox = &hitbox.rect;
      combatant->takeDamage(data);
      hits.emplace(combatant);
    }
  }
}

void KnifePiercer::endLag() {
  float percentage = 1.0 - state_clock;
  movement(percentage);

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
