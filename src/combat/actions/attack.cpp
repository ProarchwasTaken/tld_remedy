#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <raylib.h>
#include <raymath.h>
#include <set>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/combatant_event.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "system/sprite_atlas.h"
#include "combat/system/cbt_handler.h"
#include "combat/actions/attack.h"
#include <plog/Log.h>

using std::pair, std::set;


Attack::Attack(Combatant *user, SpriteAtlas &user_atlas,
               RectEx hitbox, AtkAnimSet &anim_set): 
  CombatAction(ActionID::ATTACK, ActionType::OFFENSE_MP, user, 
               0.10, 0.05, 0.25)
{
  name = "Attack: Default";
  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;

  data.damage_type = DamageType::MORALE;
  data.calculation = DamageType::MORALE;

  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;

  data.knockback = 90.0;
  data.hit_stop = 0.2;

  data.assailant = user;

  this->user_atlas = &user_atlas;
  this->anim_set = &anim_set;
  updateAnimFrameDuration();
  sendWarning(false);
}

Attack::Attack(Combatant *user, ActionType type, float wind_up, 
               float action, float end_lag, RectEx hitbox, 
               DamageData &data, SpriteAtlas &user_atlas, 
               AtkAnimSet &anim_set, bool punishable): 
  CombatAction(ActionID::ATTACK, type, user, wind_up, action, end_lag)
{
  name = "Attack: Modified";
  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;

  this->data = data;

  this->user_atlas = &user_atlas;
  this->anim_set = &anim_set;
  updateAnimFrameDuration();
  sendWarning(punishable);
}

void Attack::sendWarning(bool punishable) {
  if (user->target == NULL) {
    return;
  }

  bool dead = user->target->state == CombatantState::DEAD;
  Rectangle *target_hurtbox = &user->target->hurtbox.rect;
  if (dead || CheckCollisionRecs(hitbox.rect, *target_hurtbox)) {
    Combatant *target = user->target;

    PLOGD << "Sending warning event for: '" << target->name << "' [ID: "
      << target->entity_id << "]";
    CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING,
                                        user->target, type, wind_time,
                                        act_time, punishable);
  }
}

void Attack::updateAnimFrameDuration() {
  anim_set->wind_up.frame_duration = wind_time * 0.5;
  anim_set->end_lag.frame_duration = end_time * 0.5;
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

  set<pair<float, Combatant*>> hits;
  hitRegistration(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }
}

void Attack::hitRegistration(set<pair<float, Combatant*>> &hits) {
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
      float distance = user->distanceTo(combatant);
      hits.emplace(std::make_pair(distance, combatant));
      PLOGD << "Attack hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void Attack::inflictDamage(set<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());

  Combatant *victim;
  if (hits.size() > 1) {
    auto closest = std::min_element(hits.begin(), hits.end(), 
                                    Comparison::combatantDistance);
    victim = closest->second;
  }
  else {
    victim = hits.begin()->second;
  }
  PLOGD << "Victim selected: '" << victim->name << "' [ID: " << 
    victim->entity_id << "]";

  data.hitbox = &hitbox.rect;
  victim->takeDamage(data);

  end_time = 0.1;
  updateAnimFrameDuration();
  attack_connected = true;
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

