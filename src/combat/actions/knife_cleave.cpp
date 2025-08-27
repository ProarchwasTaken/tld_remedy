#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "system/sprite_atlas.h"
#include "utils/animation.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/knife_cleave.h"


KnifeCleave::KnifeCleave(Mary *user): 
  CombatAction(ActionID::KNIFE_CLEAVE, ActionType::OFFENSE_HP, user, 
               0.20, 0.05, 0.25) 
{
  name = "Knife Cleave";

  hitbox.scale = {28, 24};
  hitbox.offset = {-14 + (14.0f * user->direction), -50};
  user->rectExCorrection(hitbox);

  data.damage_type = DamageType::LIFE;
  data.calulation = DamageType::LIFE;

  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;

  data.knockback = 45.0;
  data.hit_stop = 0.2;

  data.assailant = user;
  
  this->atlas = &Mary::atlas;
  user->sprite = &atlas->sprites.at(20);
  updateAnimFrameDuration();

  canceled_into = cancelCheck();
  if (canceled_into) {
    wind_time = wind_time * 0.5;
  }
}

KnifeCleave::~KnifeCleave() {
  user->animation = NULL;
}

bool KnifeCleave::cancelCheck() {
  if (user->action != nullptr && !user->action->finished) {
    return true;
  }
  else {
    return false;
  }
}

void KnifeCleave::updateAnimFrameDuration() {
  anim_end.frame_duration = end_time * 0.25;
}

void KnifeCleave::windUp() {
  if (canceled_into) {
    float percentage = 1.0 - state_clock;
    float speed = (128 * percentage) * user->speed_multiplier;

    user->position.x += (speed * user->direction) * Game::deltaTime();
    user->rectExCorrection(user->bounding_box, user->hurtbox, hitbox);
  }

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[21];
    user->sfx.play("knife_cleave");
  }
}

void KnifeCleave::action() {
  if (attack_connected) {
    return;
  }

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

    Rectangle *hurtbox = &combatant->hurtbox.rect;
    if (CheckCollisionRecs(hitbox.rect, *hurtbox)) {
      data.hitbox = &hitbox.rect;
      combatant->takeDamage(data);
      attack_connected = true;

      end_time = 0.125;
      updateAnimFrameDuration();
      break;
    }
  } 
}

void KnifeCleave::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}

void KnifeCleave::drawDebug() {
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
