#pragma once
#include <string>
#include <memory>
#include <raylib.h>
#include <set>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"
#include "system/sound_atlas.h"

class CombatAction;
struct DamageData;


/* Combatants are the main focus of the CombatScene. They are otherwise
 * known as CombatActors due the common resemblance with they share with 
 * the Actor class. To summarize, a Combatant is classified as any 
 * entity capable of performing "Actions"; Along with taking and 
 * inflicting damage.*/
class Combatant : public Entity {
public:
  inline static std::set<Combatant*> existing_combatants;
  static SoundAtlas sfx;
  static Combatant *getCombatantByID(int entity_id);

  Combatant(std::string name, CombatantTeam team, Vector2 position, 
            Direction direction);
  ~Combatant();

  virtual void behavior() = 0;

  virtual void takeDamage(DamageData &data);
  float damageCalulation(DamageData &data);

  virtual void damageLife(float magnitude);

  virtual void damageMorale(float magnitude);
  virtual void increaseMorale(float magnitude);

  void enterHitstun(DamageData &data);
  void stunLogic();
  void applyKnockback();
  void exitHitstun();

  void performAction(std::unique_ptr<CombatAction> &action);
  void cancelAction();

  virtual void drawDebug() override;

  std::string name;
  CombatantTeam team;
  CombatantState state;
  Direction direction;
  RectEx hurtbox;

  float life; 
  float max_life;
  bool critical_life = false;

  int offense;
  int defense;
  int intimid;
  int persist;

  float speed_multiplier = 1.0;

  std::unique_ptr<CombatAction> action;
protected:
  DamageType damage_type;
  static constexpr float LOW_LIFE_THRESHOLD = 0.30;
private:
  float stun_time = 0;
  float stun_clock = 0.0;

  float knockback = 0;
  Direction kb_direction;
};
