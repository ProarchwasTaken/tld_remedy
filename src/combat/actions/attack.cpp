#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include <plog/Log.h>


Attack::Attack(Combatant *user, SpriteAtlas &user_atlas,
               RectEx hitbox, AttackAnimSet &anim_set): 
  CombatAction(ActionID::ATTACK, ActionType::OFFENSE_MP, user, 
               0.10, 0.05, 0.10)
{
  name = "Attack";
  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;

  data.damage_type = DamageType::MORALE;
  data.calulation = DamageType::MORALE;

  data.stun_time = 0.35;
  data.stun_type = StunType::NORMAL;

  data.knockback = 90.0;
  data.hit_stop = 0.2;
  data.assailant = user;

  this->user_atlas = &user_atlas;
  this->anim_set = &anim_set;
}

void Attack::windUp() {
  SpriteAnimation::play(user->animation, &anim_set->wind_up, false);
  int current_frame = *user->animation->current;

  user->sprite = &user_atlas->sprites[current_frame];

  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->sprite = &user_atlas->sprites[anim_set->atk_sprite];
    Combatant::sfx.play("attack");
  }
}

void Attack::action() {
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

    if (CheckCollisionRecs(hitbox.rect, combatant->hurtbox.rect)) {
      combatant->takeDamage(data);
      attack_connected = true;
      break;
    }
  }
}

void Attack::endLag() {
  SpriteAnimation::play(user->animation, &anim_set->end_lag, false);
  int current_frame = *user->animation->current;
  user->sprite = &user_atlas->sprites[current_frame];
}

void Attack::drawDebug() {
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

