#include <cassert>
#include <cstddef>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include "enums.h"
#include "game.h"
#include "data/damage.h"
#include "data/combat_event.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "combat/system/evt_handler.h"
#include "base/enemy.h"
#include <plog/Log.h>

using std::string;
int Enemy::member_count = 0;
int Enemy::stunned = 0;
int Enemy::combo = 0;


void Enemy::updateStunnedCount() {
  if (member_count == 0) {
    return;
  }

  stunned = 0;

  for (Combatant *combatant : existing_combatants) {
    if (combatant->team != CombatantTeam::ENEMY) {
      continue;
    }

    if (combatant->state == HIT_STUN || combatant->state == DEAD) {
      stunned++;
    }
  }

  if (combo > 0 && stunned == 0) {
    PLOGI << "Combo has been broken. Score: " << combo;
    combo = 0;
  }
}


Enemy::Enemy(string name, EnemyID id, Vector2 position): 
  Combatant(name, CombatantTeam::ENEMY, position, LEFT)
{
  this->id = id;
  member_count++;
}

Enemy::~Enemy() {
  member_count--;
  assert(member_count >= 0);
}

void Enemy::takeDamage(DamageData &data) {
  if (data.damage_type == DamageType::LIFE) {
    float percentage = combo / 15.0;
    percentage = Clamp(percentage, 0.0, 1.0);

    float bonus = std::powf(percentage, 2);
    data.power = data.power + bonus; 
  }

  Combatant::takeDamage(data);

  if (data.assailant->team != CombatantTeam::PARTY) {
    return;
  }

  PartyMember *combatant = static_cast<PartyMember*>(data.assailant);
  if (!combatant->important && !data.force_hitstop) {
    return;
  }

  if (data.damage_type == DamageType::LIFE) {
    Game::sleep(data.hit_stop);
  }
}

void Enemy::finalIntercept(float &damage, DamageData &data) {
  if (data.damage_type == DamageType::MORALE) {
    float damage_mod = 1.0 - (combo / 18.0);
    damage_mod = Clamp(damage_mod, 0.0, 1.0);

    damage = damage * damage_mod;
    return;
  }
}

void Enemy::enterHitstun(DamageData &data) {
  assert(state != DEAD);
  combo++;
  PLOGI << "Combo: " << combo;

  if (state == ACTION && counterToastCondition(data)) {
    PLOGD << "Starting combat toast: Counter";
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 0);
  }

  Combatant::enterHitstun(data);
}

void Enemy::enterHitstun(float seconds, StunType type, Color tint) {
  assert(state != DEAD);
  combo++;
  PLOGI << "Combo: " << combo;

  Combatant::enterHitstun(seconds, type, tint);
}

bool Enemy::counterToastCondition(DamageData &data) {
  if (data.assailant == NULL && data.assailant->team == team) {
    return false;
  }

  PartyMember *assailant = static_cast<PartyMember*>(data.assailant);
  return assailant->important || data.force_hitstop;
}

void Enemy::exitHitstun() {
  Combatant::exitHitstun();
}

void Enemy::death() {
  combo++;
  PLOGI << "Combo: " << combo;

  Combatant::death();
}
