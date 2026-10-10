#include <cassert>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <memory>
#include <random>
#include <set>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "base/status_effect.h"
#include "data/combatant_event.h"
#include "utils/animation.h"
#include "utils/comparisons.h"
#include "combat/system/cbt_handler.h"
#include "combat/combatants/party/xander.h"
#include "combat/status_effects/broken_arm.h"
#include "combat/status_effects/crippled_leg.h"
#include "combat/status_effects/mangled.h"
#include "combat/actions/meteor.h"
#include <plog/Log.h>

using std::pair, std::set, std::uniform_real_distribution,
      std::uniform_int_distribution, std::make_unique, std::unique_ptr;


Meteor::Meteor(Xander *user) : 
  CombatAction(ActionID::XANDER_METEOR, ActionType::OFFENSE_HP, user, 
      1.2, 0.10, 1.35)
{
  name = "Meteor";
  this->user = user;
  this->atlas = &Xander::atlas;

  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;

  data.stun_time = 1.0;
  data.stun_type = StunType::STAGGER;

  data.assailant = user;
  data.hitbox = &hitbox.rect;

  data.power = 1.5;

  data.hit_stop = 0.20;
  data.apply_hitstop = true;

  hitbox.scale = {18, 48};
  hitbox.offset = {-9.0f + (44.0f * user->direction), -68};
  warning_hitbox.scale = {128, 72};
  warning_hitbox.offset = {-64.0f + (64.0f * user->direction), -72};
  user->rectExCorrection(hitbox, warning_hitbox);

  sendWarning();
}

Meteor::~Meteor() {
  user->animation = NULL;
}

void Meteor::sendWarning() {
  Combatant *target = user->target;
  CombatantHandler::queue<WarningCBT>(user, CombatantEVT::WARNING, target,
                                      type, warning_hitbox.rect, 
                                      wind_time, act_time, user, true);
}

void Meteor::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[26];
  }
}

void Meteor::action() {
  if (attack_connected) {
    return;
  }

  set<pair<float, Combatant*>> hits;
  hitRegistration(hits);

  if (!hits.empty()) {
    inflictDamage(hits);
  }
}

void Meteor::hitRegistration(set<pair<float, Combatant*>> &hits) {
  for (Combatant *combatant: Combatant::existing_combatants) {
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
      PLOGD << "Meteor hitbox has collided with Combatant [ID: " <<
      combatant->entity_id << "], Distance: " << distance;
    }
  }
}

void Meteor::inflictDamage(set<pair<float, Combatant*>> &hits) {
  assert(!hits.empty());

  Combatant *victim;
  data.hitbox = &hitbox.rect;

  if (hits.size() > 1) {
    auto closest = std::min_element(hits.begin(), hits.end(), 
                                    Comparison::combatantPriority);
    victim = closest->second;
  }
  else {
    victim = hits.begin()->second;
  }
  PLOGD << "Victim selected: '" << victim->name << "' [ID: " << 
    victim->entity_id << "]";

  victim->takeDamage(data);

  if (victim->state == HIT_STUN) {
    attemptBoneBreak(victim);
  }

  attack_connected = true;
}

void Meteor::attemptBoneBreak(Combatant *victim) {
  PLOGI << "Running bone break chance...";
  uniform_real_distribution<float> range(0.0, 1.0);

  float break_chance = 1.0 - victim->resilience;
  float break_roll = range(Game::RNG);

  if (break_roll > break_chance) {
    return;
  }

  uniform_int_distribution<int> effect_range(0, 2);
  int effect_roll = effect_range(Game::RNG);
  PLOGD << "Status Ailment selected: " << effect_roll;

  StatusID selected_effect = static_cast<StatusID>(effect_roll);
  for (auto &effect : victim->status) {
    if (effect->id == selected_effect) {
      PLOGI << "Victim already has that status effect.";
      return;
    }
  }

  unique_ptr<StatusEffect> status_effect = nullptr;
  switch (selected_effect) {
    case StatusID::BROKEN_ARM: {
      status_effect = make_unique<BrokenArm>(victim);
      break;
    }
    case StatusID::CRIPPLED_LEG: {
      status_effect = make_unique<CrippledLeg>(victim);
      break;
    }
    case StatusID::MANGLED: {
      status_effect = make_unique<Mangled>(victim);
      break;
    }
    default: {
      PLOGE << "Invalid ID!";
      break;
    } 
  }

  assert(status_effect != nullptr);
  victim->afflictStatus(status_effect);
}

void Meteor::endLag() {
  float elapsed_time = end_time * state_clock;
  if (elapsed_time < 0.10) {
    user->sprite = &atlas->sprites[27];
    return;
  }

  SpriteAnimation::play(user->animation, &anim_end, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->animation = NULL;
    user->intended_pos = user->position;
  }
}

void Meteor::drawDebug() {
  Color color = ORANGE;
  DrawRectangleLinesEx(warning_hitbox.rect, 1, color);

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
