#include <cassert>
#include <algorithm>
#include <utility>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/collision.h"
#include "utils/comparisons.h"
#include "combat/system/cbt_handler.h"
#include "combat/combatants/party/xander.h"
#include "combat/actions/tail_whip.h"
#include <plog/Log.h>

using std::pair, std::vector;


TailWhip::TailWhip(Xander *user) :
  CombatAction(ActionID::XANDER_TAILWHIP, ActionType::OFFENSE_HP, user, 
      0.8, 0.15, 0.6)
{
  name = "Tail Whip";
  this->user = user;
  this->atlas = &Xander::atlas;
  this->sfx = &Xander::psfx;
  assert(sfx->users() > 0);

  whip_hitbox.scale = {72, 32};
  whip_hitbox.offset = {-36.0f + (36.5f * user->direction), -51};
  body_hitbox.scale = {70, 40};
  body_hitbox.offset = {-30, -44};
  user->rectExCorrection(whip_hitbox, body_hitbox);

  atk = user->offense + user->dexterity;
  initBodyData();
  sendWarning();
  sfx->play("xander_twirl");
}

TailWhip::~TailWhip() {
  user->animation = NULL;

  if (!finished) {
    user->intended_pos = user->position;
  }
}

void TailWhip::initBodyData() {
  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 0.40;
  data.stun_type = StunType::NORMAL;

  data.knockback = 200;

  data.assailant = user;
  data.hitbox = &body_hitbox.rect;

  data.a_atk = &atk;
  data.power = 0.5;

  data.hit_stop = 0.0;
  data.apply_hitstop = false;
}

void TailWhip::initWhipData() {
  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 0.60;
  data.stun_type = StunType::NORMAL;

  data.knockback = 160;

  data.assailant = user;
  data.hitbox = &whip_hitbox.rect;

  data.a_atk = &atk;
  data.power = 0.5;

  data.hit_stop = 0.25;
  data.apply_hitstop = true;
}

void TailWhip::sendWarning() {
  Combatant *target = user->target;
  CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING, target,
                                      type, whip_hitbox.rect, wind_time,
                                      act_time, user, true);
}

void TailWhip::windUp() {
  float elapsed_time = state_clock * wind_time;
  if (elapsed_time < 0.2) {
    user->sprite = &atlas->sprites[10];
    return;
  }

  if (!performed_check) {
    bodyCheck();
    initWhipData();
    performed_check = true;
  }

  lungeMovement();
  user->rectExCorrection(user->hurtbox, user->bounding_box, whip_hitbox);

  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[18];
    sfx->play("xander_whip");
  }
}

void TailWhip::bodyCheck() {
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

    if (!CheckCollisionRecs(body_hitbox.rect, combatant->hurtbox.rect)) {
      continue;
    }

    combatant->takeDamage(data);
    if (combatant->state == HIT_STUN) {
      int dir_to = combatant->directionTo(user);
      Direction direction = static_cast<Direction>(dir_to * -1);
      combatant->setKnockback(data.knockback, data.stun_time, direction);
    }
  }
}

void TailWhip::lungeMovement() {
  float percentage = 1.0 - state_clock;
  float speed = 160 * percentage;

  float multiplier = user->speed_multiplier;
  float magnitude = speed * multiplier * Game::deltaTime();
  Direction direction = user->direction;

  if (Collision::checkX(user, magnitude, direction)) {
    Collision::snapX(user, direction);
  }
  else {
    user->position.x += magnitude * direction;
  }
}

void TailWhip::action() { 
  if (attack_connected) {
    return;
  }

  vector<pair<float, Combatant*>> hits;
  whipHitReg(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }
}

void TailWhip::whipHitReg(vector<pair<float, Combatant*>> &hits) {
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

    if (CheckCollisionRecs(whip_hitbox.rect, combatant->hurtbox.rect)) {
      float distance = user->distanceTo(combatant);
      hits.push_back(std::make_pair(distance, combatant));
      PLOGD << "Tail hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void TailWhip::inflictDamage(vector<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());
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

void TailWhip::endLag() {
  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->intended_pos = user->position;
  }
}

void TailWhip::drawDebug() {
  Color color;
  if (phase == ActionPhase::WIND_UP) {
    color = PINK;
    color.a = 128;
    DrawRectangleLinesEx(body_hitbox.rect, 1, color);
  }

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
  DrawRectangleRec(whip_hitbox.rect, color);
}
