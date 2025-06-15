#pragma once
#include <string>
#include <raylib.h>
#include <set>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"


/* Combatants are the main focus of the CombatScene. They are otherwise
 * known as CombatActors due the common resemblance with they share with 
 * the Actor class. To summarize, a Combatant is classified as any 
 * entity capable of performing "Actions"; Along with taking and 
 * inflicting damage.*/
class Combatant : public Entity {
public:
  inline static std::set<Combatant*> existing_combatants;
  static Combatant *getCombatantByID(int entity_id);
  static int enemyCount() {return enemy_count;}

  Combatant(std::string name, CombatantType combatant_type,
            Vector2 position, Direction direction);
  ~Combatant();

  virtual void behavior() = 0;
  virtual void drawDebug() override;

  std::string name;
  CombatantType combatant_type;
  CombatantState state;
  Direction direction;
  RectEx hurtbox;

  float life; 
  float max_life;

  int offense;
  int defense;
  int intimid;
  int persist;

  float speed_multiplier = 1.0;
private:
  static int enemy_count;
};
