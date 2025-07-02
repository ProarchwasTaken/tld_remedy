#include <raylib.h>
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "game.h"
#include "base/enemy.h"

using std::string;
int Enemy::member_count = 0;


Enemy::Enemy(string name, EnemyID id, Vector2 position): 
  Combatant(name, CombatantTeam::ENEMY, position, LEFT)
{
  this->id = id;
  member_count++;
}

Enemy::~Enemy() {
  member_count--;
}

void Enemy::takeDamage(DamageData &data) {
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
