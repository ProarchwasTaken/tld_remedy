#include <cassert>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <set>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "combat/system/cbt_handler.h"
#include "combat/combatants/party/erwin.h"
#include "combat/actions/3rd_party.h"
#include <plog/Log.h>

using std::set, std::pair;


ThirdParty::ThirdParty(Erwin *user): 
  CombatAction(ActionID::ERWIN_3RD_PARTY, ActionType::OFFENSE_HP, user, 
               0.30, 0.10, 0.50)
{
  name = "3rd Party";
  this->user = user;
  this->atlas = &user->atlas;

  hitbox.scale = {32, 7};
  hitbox.offset = {-16 + (16.0f * user->direction), -35};
  user->rectExCorrection(hitbox);

  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 1.0;
  data.stun_type = StunType::NORMAL;

  data.atk_mod = 1.30;
  data.knockback = 200.0;

  data.hit_stop = 0.25;
  data.force_hitstop = true;

  data.assailant = user;

  sendWarning();

  sfx = &user->psfx;
  sfx->play("3rdparty_jump");
}

ThirdParty::~ThirdParty() {
  user->animation = NULL;
}

void ThirdParty::sendWarning() {
  if (user->target == NULL) {
    return;
  }

  bool dead = user->target->state == CombatantState::DEAD;
  if (!dead) {
    Combatant *target = user->target;

    PLOGD << "Sending warning event for: '" << target->name << "' [ID: "
      << target->entity_id << "]";
    CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING,
                                        user->target, type, hitbox.rect,
                                        wind_time, act_time, user);
  }
}

void ThirdParty::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  user->movement(user->speed_multiplier * 3);
  user->rectExCorrection(user->bounding_box, user->hurtbox, hitbox);

  bool end_state = state_clock == 1.0;
  if (end_state) {
    user->sprite = &atlas->sprites[27];
    sfx->play("3rdparty_dropkick");
  }
}

void ThirdParty::action() {
  if (attack_connected) {
    return;
  }

  set<pair<float, Combatant*>> hits;
  hitRegistration(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }

  bool end_state = state_clock == 1.0;
  if (end_state) {
    sfx->play("3rdparty_end");
  }
}

void ThirdParty::hitRegistration(set<pair<float, Combatant*>> &hits) {
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

void ThirdParty::inflictDamage(set<pair<float, Combatant*>> &hits) {
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

  attack_connected = true;
  sfx->play("3rdparty_hit");
}

void ThirdParty::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}

void ThirdParty::drawDebug() {
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
