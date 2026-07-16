#include <cassert>
#include <algorithm>
#include <utility>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "combat/combatants/party/mary.h"
#include "combat/system/cbt_handler.h"
#include "combat/actions/bat_swing.h"
#include <plog/Log.h>

using std::pair, std::vector;


BatSwing::BatSwing(Mary *user) : 
  CombatAction(ActionID::BAT_SWING, ActionType::OFFENSE_HP, user, 
               0.5, 0.10, 0.5)
{
  this->user = user;
  this->atlas = &Mary::atlas;

  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;
  
  data.knockback = 70.0;
  data.hit_stop = 0.25;

  data.assailant = user;

  hitbox.scale = {38, 17};
  updateHitboxOffset();
  user->rectExCorrection(hitbox);

  sendWarning();
}

BatSwing::~BatSwing() {
  user->animation = NULL;
}

void BatSwing::sendWarning() {
  RectEx warning_hitbox = hitbox;
  warning_hitbox.scale = {54, 17};
  warning_hitbox.offset = {-27.0f + (35.0f * user->direction), -43};
  user->rectExCorrection(warning_hitbox);

  Combatant *target = user->target;
  CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING,
                                      user->target, type, 
                                      warning_hitbox.rect, wind_time, 
                                      act_time, user, true);
}

void BatSwing::updateHitboxOffset() {
  hitbox.offset = {-19.0f + (26.0f * user->direction), -43};
}

void BatSwing::windUp() {
  if (state_clock >= 0.4) {
    freeTurning();
  }

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
  if (attack_connected) {
    return;
  }

  vector<pair<float, Combatant*>> hits;
  hitRegistration(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }
}

void BatSwing::hitRegistration(vector<pair<float, Combatant*>> &hits) {
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
      hits.push_back(std::make_pair(distance, combatant));
      PLOGD << "Swing hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void BatSwing::inflictDamage(vector<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());

  data.hitbox = &hitbox.rect;
  std::sort(hits.begin(), hits.end(), Comparison::combatantPriority);

  int weight = attack_weight;
  PLOGD << "Initial Attack Weight: " << weight;
  PLOGD << "Total Number of Hits: " << hits.size();

  for (auto &hit : hits) {
    Combatant *victim = hit.second;
    victim->takeDamage(data);

    weight = weight - victim->priority;
    PLOGD << "Victim Priority: " << victim->priority;
    PLOGD << "Attack Weight has been lowered to: " << weight; 

    if (weight <= 0) {
      break;
    }
  }

  attack_connected = true;
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
