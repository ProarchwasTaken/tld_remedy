#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/combatant.h"
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
