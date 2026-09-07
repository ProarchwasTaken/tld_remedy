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
#include "combat/system/cbt_handler.h"
#include "combat/combatants/party/xander.h"
#include "combat/actions/hand_blade.h"
#include <plog/Log.h>

using std::pair, std::vector;


HandBlade::HandBlade(Xander *user) : 
  CombatAction(ActionID::XANDER_HANDBLADE, ActionType::OFFENSE_HP, user, 
      0.5, 0.1, 0.6)
{
  name = "Hand Blade";
  this->atlas = &Xander::atlas;
  this->sfx = &Xander::psfx;
  assert(sfx->users() > 0);

  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;

  data.knockback = 110;
  data.assailant = user;

  hitbox.scale = {51, 32};
  hitbox.offset = {-25.5f + (25.5f * user->direction), -61};

  warning_box.scale = {72, 48};
  warning_box.offset = {-36.0f + (36.0f * user->direction), -69};
  user->rectExCorrection(hitbox, warning_box);

  sfx->play("xander_growl");
}

HandBlade::~HandBlade() {
  user->animation = NULL;
}

void HandBlade::sendWarning() {
  Combatant *target = user->target;
  CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING, target,
                                      type, warning_box.rect, wind_time,
                                      act_time, user, true);
}

void HandBlade::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[11];
    sfx->play("xander_handblade");
  }
}

void HandBlade::action() {
  if (attack_connected) {
    return;
  }

  vector<pair<float, Combatant*>> hits;
  hitRegistration(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }
}

void HandBlade::hitRegistration(vector<pair<float, Combatant*>> &hits) {
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
      PLOGD << "Hand Blade hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void HandBlade::inflictDamage(vector<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());

  data.hitbox = &hitbox.rect;
  std::sort(hits.begin(), hits.end(), Comparison::combatantPriority);

  int weight = attack_weight;
  PLOGD << "Initial Attack Weight: " << weight;
  PLOGD << "Total Number of Hits: " << hits.size();

  for (auto &hit : hits) {
    Combatant *victim = hit.second;
    victim->takeDamage(data);

    if (user->state == HIT_STUN) {
      return;
    }

    weight = weight - victim->priority;
    PLOGD << "Victim Priority: " << victim->priority;
    PLOGD << "Attack Weight has been lowered to: " << weight; 

    if (weight <= 0) {
      break;
    }
  }

  attack_connected = true;
}

void HandBlade::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}

void HandBlade::drawDebug() {
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
  DrawRectangleLinesEx(warning_box.rect, 1, ORANGE);
  DrawRectangleRec(hitbox.rect, color);
}
