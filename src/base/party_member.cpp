#include <raylib.h>
#include <raymath.h>
#include <string>
#include "enums.h"
#include "base/combatant.h"
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
