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


Enemy::Enemy(string name, EnemyID id, Vector2 position): 
  Combatant(name, CombatantTeam::ENEMY, position, LEFT)
{
  this->id = id;
  member_count++;
}

Enemy::~Enemy() {
  member_count--;
  assert(member_count >= 0);

  if (state == DEAD) {
    assert(stunned > 0);
    stunned--;
  }
}

void Enemy::takeDamage(DamageData &data) {
  if (stunned == 0) {
    PLOGI << "Reseting Combo Count.";
    combo = 0;
  }

  if (data.damage_type == DamageType::LIFE) {
    float percentage = combo / 15.0;
    percentage = Clamp(percentage, 0.0, 1.0);

    float modifier = 1.0 - std::powf(percentage, 2);
    data.def_mod = data.def_mod * modifier; 
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
  if (state != HIT_STUN) {
    stunned = Clamp(stunned + 1, 0, member_count);
  }

  combo++;
  PLOGI << "Combo: " << combo;

  if (state == ACTION && counterToastCondition(data)) {
    PLOGD << "Starting combat toast: Counter";
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 0);
  }

  Combatant::enterHitstun(data);
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

  assert(stunned > 0);
  stunned--;
}

void Enemy::death() {
  if (state != HIT_STUN) {
    stunned++;
  }

  combo++;
  PLOGI << "Combo: " << combo;

  Combatant::death();
}
