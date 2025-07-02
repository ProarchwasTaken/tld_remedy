#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"


/* The Enemy subclass is used by Combatants who directly oppose the 
 * player, and Combatants who side with them.*/
class Enemy : public Combatant {
public:
  static int memberCount() {return member_count;}
  Enemy(std::string name, EnemyID id, Vector2 position);
  ~Enemy();

  void takeDamage(DamageData &data) override;

  EnemyID id;
private:
  static int member_count;
};
