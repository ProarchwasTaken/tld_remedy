#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <random>
#include <string>
#include <memory>
#include <set>
#include "game.h"
#include "enums.h"
#include "data/damage.h"
#include "data/session.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "base/party_member.h"
#include "combat/status_effects/crippled_arm.h"
#include "combat/status_effects/crippled_leg.h"
#include "combat/status_effects/broken.h"
#include "combat/status_effects/despondent.h"
#include "combat/system/stage.h"
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

bool PartyMember::isEnabled() {
  return enabled;
}

void PartyMember::setEnabled(bool value) {
  enabled = value;

  if (enabled) {
    PLOGI << "PartyMember: '" << name << "' behavior has been enabled.";
  }
  else {
    PLOGI << "PartyMember: '" << name << "' behavior has been disabled."; 
  }
}

void PartyMember::takeDamage(DamageData &data) {
  bool not_demoralized = !demoralized;
  Combatant::takeDamage(data);

  deplete_clock = 0.0;
  deplete_delay = DEFAULT_DEPLETE_DELAY;

  if (!important) {
    return;
  }

  bool apply_hitstop = false;

  if (data.damage_type == DamageType::LIFE) {
    apply_hitstop = true;
  }

  bool became_demoralized = not_demoralized == demoralized;
  if (became_demoralized) {
    data.hit_stop *= 2;
    apply_hitstop = true;
    CombatStage::tintStage(Game::palette[40]);
  }

  if (apply_hitstop) {
    Game::sleep(data.hit_stop);
  }
}

void PartyMember::finalIntercept(float &damage, DamageData &data) {
  if (critical_life || data.damage_type != DamageType::LIFE) {
    return;
  }

  bool fatal_damage = life - damage <= 0;
  if (fatal_damage) {
    PLOGI << "Applying Death Protection.";
    float average = (damage + life) / 2 ;
    float reduction = 1.0 - ((max_life / average) * 0.20);
    damage = damage * reduction;

    PLOGD << "Result: " << damage;
  }
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
  afflictPersistent(id);
}

void PartyMember::afflictPersistent(StatusID id, bool hide_text) {
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

  afflictStatus(status_effect, hide_text);
}

void PartyMember::afflictPersistent(StatusID status[STATUS_LIMIT]) {
  for (int x = 0; x < STATUS_LIMIT; x++) {
    StatusID status_id = status[x];
    if (status_id != StatusID::NONE) {
      afflictPersistent(status_id, true);
    } 
  }
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

  if (!demoralized && morale < 0) {
    PLOGI << "Morale is below 0!";
    unique_ptr<StatusEffect> effect = make_unique<Despondent>(this);
    afflictStatus(effect);
    sfx.play("despondent");
  }
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

void PartyMember::tintFlash() {
  if (state == HIT_STUN || state == DEAD) {
    return;
  }

  Color start_tint;
  if (critical_life) {
    start_tint = Game::palette[35];
  }
  else if (demoralized) {
    start_tint = Game::palette[43];
  }
  else {
    tint = WHITE;
    return;
  }

  float sine = std::sinf(GetTime() * 15);
  float percentage = (sine * 0.5) + 0.5;

  unsigned char r = Lerp(start_tint.r, 255, percentage);
  unsigned char g = Lerp(start_tint.g, 255, percentage);
  unsigned char b = Lerp(start_tint.b, 255, percentage);

  tint = {r, g, b, 255};
}
