#include <cassert>
#include <raylib.h>
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "game.h"
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

  Combatant::takeDamage(data);

  if (data.assailant->team != CombatantTeam::PARTY) {
    return;
  }

  PartyMember *combatant = static_cast<PartyMember*>(data.assailant);
  if (!combatant->important) {
    return;
  }

  if (data.damage_type == DamageType::LIFE) {
    Game::sleep(data.hit_stop);
  }
}

void Enemy::enterHitstun(DamageData &data) {
  assert(state != DEAD);
  if (state != HIT_STUN) {
    stunned++;
  }

  if (data.stun_type != StunType::DEFENSIVE) {
    combo++;
    PLOGI << "Combo: " << combo;
  }

  Combatant::enterHitstun(data);
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
