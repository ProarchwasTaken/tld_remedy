#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <random>
#include <string>
#include <memory>
#include <set>
#include "game.h"
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "base/party_member.h"
#include "combat/status_effects/crippled_arm.h"
#include "combat/status_effects/crippled_leg.h"
#include "combat/status_effects/broken.h"
#include <plog/Log.h>

using std::string, std::set, std::uniform_int_distribution, 
std::unique_ptr, std::make_unique;
int PartyMember::member_count = 0;


PartyMember::PartyMember(string name, PartyMemberID id, Vector2 position): 
  Combatant(name, CombatantTeam::PARTY, position, RIGHT)
{
  this->id = id;
  member_count++;
}

PartyMember::~PartyMember() {
  member_count--;
}

void PartyMember::takeDamage(DamageData &data) {
  Combatant::takeDamage(data);

  if (important && data.damage_type == DamageType::LIFE) {
    Game::sleep(data.hit_stop);
  }

  deplete_clock = 0.0;
  deplete_delay = DEFAULT_DEPLETE_DELAY;
}

void PartyMember::damageLife(float magnitude) {
  bool in_critical = critical_life;

  Combatant::damageLife(magnitude);

  bool entered_critical = in_critical != critical_life;
  if (entered_critical && state != CombatantState::DEAD) {
    afflictPersistent();
  }
}

void PartyMember::afflictPersistent() {
  set<StatusID> effect_pool = getEffectPool();

  PLOGI << "Possible status effect to inflict: " << effect_pool.size();
  if (effect_pool.empty()) {
    PLOGI << "PartyMember has reached the end of their rope!";
    life = 0;
    death();
    return;
  }

  StatusID id = selectRandomID(effect_pool);
  unique_ptr<StatusEffect> status_effect = nullptr;

  switch (id) {
    case StatusID::CRIPPLED_ARM: {
      status_effect = make_unique<CrippledArm>(this);
      break;
    }
    case StatusID::CRIPPED_LEG: {
      status_effect = make_unique<CrippledLeg>(this);
      break;
    }
    case StatusID::BROKEN: {
      status_effect = make_unique<Broken>(this);
      break;
    }
    default: {
      PLOGE << "Invalid ID!";
      break;
    }
  }

  afflictStatus(status_effect);
}

set<StatusID> PartyMember::getEffectPool() {
  set<StatusID> pool = {
    StatusID::CRIPPLED_ARM, 
    StatusID::CRIPPED_LEG,
    StatusID::BROKEN
  };

  for (auto &status_effect : status) {
    pool.erase(status_effect->id);
  }

  return pool;
}

StatusID PartyMember::selectRandomID(set<StatusID> &effect_pool) {
  StatusID id;
  int count = effect_pool.size();
  if (count == 1) {
    id = *effect_pool.begin();
  }
  else {
    StatusID pool[count];
    std::copy(effect_pool.begin(), effect_pool.end(), pool);

    uniform_int_distribution<int> range(0, count - 1); 
    int index = range(Game::RNG);
    PLOGD << "Retrieving effect id at index: " << index;

    id = pool[index];
  }

  return id;
}

void PartyMember::damageMorale(float magnitude) {
  float new_morale = morale - magnitude;
  morale = Clamp(new_morale, -max_morale, max_morale);
  PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
    "] Morale has been decreased to: " << morale;
}

void PartyMember::increaseMorale(float magnitude) {
  float new_morale = morale + magnitude;
  morale = Clamp(new_morale, -max_morale, max_morale);
  PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
    "] Morale has been increased to: " << morale;
}

void PartyMember::increaseExhaustion(float magnitude) {
  float destined_life = life - magnitude;
  bool too_much = destined_life < 1.0;

  magnitude = magnitude - (too_much * ((destined_life * -1) + 1));
  PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
    "] exhaustion has been increased by: " << magnitude;

  exhaustion += magnitude;
  life -= magnitude;

  if (!critical_life && life < max_life * LOW_LIFE_THRESHOLD) {
    PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
      "] is now Winded!";
    critical_life = true;
    deplete_delay = 2.0;
  }
  else {
    deplete_delay = DEFAULT_DEPLETE_DELAY;
  }

  deplete_clock = 0.0;
}

void PartyMember::depleteExhaustion() {
  if (deplete_clock < 1.0) {
    deplete_clock += Game::deltaTime() / deplete_delay;
    return;
  }

  float magnitude = recovery * (max_life * 0.125);
  magnitude *= Game::deltaTime();

  exhaustion = Clamp(exhaustion - magnitude, 0, max_life);
  life = Clamp(life + magnitude, 0, max_life);

  if (critical_life && life >= max_life * LOW_LIFE_THRESHOLD) {
    PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
      "] is no longer Winded.";
    critical_life = false;
  }
}

void PartyMember::depleteInstant() {
  life += exhaustion;
  exhaustion = 0;

  if (critical_life && life >= max_life * LOW_LIFE_THRESHOLD) {
    PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << 
      "] is no longer Winded.";
    critical_life = false;
  }
}
