#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/combatant.h"
#include "base/party_member.h"

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

}

void PartyMember::increaseMorale(float magnitude) {

}
