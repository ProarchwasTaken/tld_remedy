#pragma once
#include <string>
#include <memory>
#include <raylib.h>
#include <set>
#include <vector>
#include "enums.h"
#include "data/rect_ex.h"
#include "base/entity.h"
#include "system/sound_atlas.h"

class CombatAction;
struct DamageData;

class StatusEffect;
typedef std::vector<std::unique_ptr<StatusEffect>> Status;


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
  Color spriteTint() {return tint;}

  float distanceTo(Combatant *combatant);

  virtual void behavior() = 0;

  /* The process of taking damage is actually a very thorough procedure. 
   * It is a crucial aspect of any Combatant's functionality afterall.
   * Built in, it has multiple intercepts of which could be used to
   * modify the given damage data, execute special code, or cancel the
   * function all together.*/
  virtual void takeDamage(DamageData &data);
  float damageCalulation(DamageData &data);
  virtual void finalIntercept(float &damage, DamageData &data) {};
  void applyDamage(float damage, DamageData &data);

  virtual void damageLife(float magnitude);

  /* This function (Along with increaseMorale) does nothing. This is 
   * because, Combatants at their core do not possess Morale attributes.
   * Rather theses functions are meant to be overwritten by Combatants
   * who do have those values. The notable example are PartyMembers.*/
  virtual void damageMorale(float magnitude);
  virtual void increaseMorale(float magnitude);

  virtual void enterHitstun(DamageData &data);
  void stunLogic();
  void applyKnockback(float clock, float minimum = 0.0);
  void stunTintLerp();
  virtual void exitHitstun();

  virtual void death();
  float deathClock() {return death_clock;}
  void deathTintLerp();
  void deathAlphaLerp();
  void deathLogic();

  /* Actions are stored on the heap due their volatility. They're not
   * static, and they could literally be anything for all the game knows.
   * As such, function must be called if you want a Combatant to perform
   * an action; With the action created and owned by a separate 
   * unique_ptr ahead of time as to ensure no problems would occur on
   * the Combatant's end.*/
  void performAction(std::unique_ptr<CombatAction> &action);
  void cancelAction();

  /* Everything said about performAction can also be mentioned with
   * this function as well. To afflict an Combatant with a status effect,
   * call this function to minimize issues.*/
  void afflictStatus(std::unique_ptr<StatusEffect> &status_effect,
                     bool hide_text = false);
  void statusLogic();
  void removeErasedStatus();

  virtual void drawDebug() override;

  std::string name;
  CombatantState state;
  CombatantTeam team;
  Combatant *target = NULL;

  Direction direction;
  RectEx hurtbox;

  float life; 
  float max_life;

  bool intangible = false;
  bool critical_life = false;

  int offense;
  int defense;
  int intimid;
  int persist;

  float speed_multiplier = 1.0;

  std::unique_ptr<CombatAction> action;
  Status status;

  static constexpr float LOW_LIFE_THRESHOLD = 0.30;
protected:
  Color tint = WHITE;
  float death_time = 1.0;

  DamageType damage_type;
private:
  float stun_time = 0;
  float stun_clock = 0.0;

  float knockback = 0;
  Direction kb_direction;
  float kb_time = 0;
  float kb_clock = 0;

  float death_clock = 0.0;

  Color start_tint;
};
