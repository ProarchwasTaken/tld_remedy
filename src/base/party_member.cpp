#include <raylib.h>
#include <raymath.h>
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"
#include "game.h"
#include "base/party_member.h"
#include <plog/Log.h>

using std::string;
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
