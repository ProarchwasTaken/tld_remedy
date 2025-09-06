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
  Enemy(std::string name, EnemyID id, Vector2 position);
  ~Enemy();

  static int memberCount() {return member_count;}
  static int comboCount() {return combo;} 

  void takeDamage(DamageData &data) override;
  void finalIntercept(float &damage, DamageData &data) override;

  void enterHitstun(DamageData &data) override;
  void exitHitstun() override;

  void death() override;

  EnemyID id;
private:
  static int member_count;

  static int stunned;
  static int combo;
};
