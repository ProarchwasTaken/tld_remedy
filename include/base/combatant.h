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
class CombatantEvent;

class StatusEffect;
typedef std::vector<std::unique_ptr<StatusEffect>> Status;

template <typename EventType>
using EventPool = std::vector<std::unique_ptr<EventType>>;

enum CombatantState {
  ACTION,
  NEUTRAL,
  HIT_STUN,
  DEAD
};

enum class DamageType {
  LIFE,
  MORALE,
};

enum class StunType {
  NORMAL,
  STAGGER
};


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

  float distanceTo(Entity *entity);

  virtual void behavior() {};
  virtual void evaluateEvent(std::unique_ptr<CombatantEvent> &event);

  void accelerate();
  void decelerate();

  /* The process of taking damage is actually a very thorough procedure. 
   * It is a crucial aspect of any Combatant's functionality afterall.
   * Built in, it has multiple intercepts of which could be used to
   * modify the given damage data, execute special code, or cancel the
   * function all together.*/
  virtual void takeDamage(DamageData &data);

  bool preDamageInterception(DamageData &data);
  float damageProcedure(DamageData &data);
  float damageCalculation(DamageData &data);

  bool useTenacity(float damage, DamageType type);
  float tpDamageCalculation(float damage);

  virtual void finalIntercept(float &damage, DamageData &data) {};
  void applyDamage(float damage, DamageData &data);

  virtual void damageLife(float magnitude);
  virtual void increaseLife(float magnitude);

  void increaseEntropy(float magnitude);
  void lifeDecay();

  void damageTenacity(float magnitude);
  void increaseTenacity(float magnitude, float threshold);
  void regenerateTenacity();

  /* This function (Along with increaseMorale) does nothing. This is 
   * because, Combatants at their core do not possess Morale attributes.
   * Rather theses functions are meant to be overwritten by Combatants
   * who do have those values. The notable example are PartyMembers.*/
  virtual void damageMorale(float magnitude);
  virtual void increaseMorale(float magnitude, bool mp_share = true);

  /* Hit Stun is a state that most combatants will enter upon taking
   * damage. In such a state, they will be unable to do anything the
   * stun ends.*/
  virtual void enterHitstun(DamageData &data);

  /* This overload forces a combatant to enter hit stun outside the
   * context of taking damage. The combatant's current action will not be
   * cleared from memory when this function is called. 
   *
   * This is to reduce the risk of clearing an action from memory while 
   * it is still running it's logic. Which may result in a seqfault. 
   * Nevertheless,this possibility must always be accounted for when 
   * running this function.*/
  virtual void enterHitstun(float seconds, StunType type, Color tint);

  void stunLogic();
  void stunTintLerp();
  virtual void exitHitstun();

  void setKnockback(float velocity, float seconds, Direction direction);
  void knockbackLogic();
  void applyKnockback(float clock, float minimum = 0.0);

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

  /* For code that should only be ran at absolute END of a combatant's
   * update cycle.*/
  virtual void endLogic();

  void applyStaggerEffect(Rectangle &final);
  virtual void drawDebug() override;

  std::string name;
  CombatantState state;

  CombatantTeam team;
  Combatant *target = NULL;

  Direction direction;
  RectEx hurtbox;

  float life; 
  float max_life;
  float entropy = 0;

  float tenacity = 0.0;
  float tp_threshold = 0.0;
  float tp_natural = 0.0;

  float tp_regen_delay = 8.0;
  float tp_regen_clock = 0.0;

  bool targetable = true;
  bool intangible = false;
  bool critical_life = false;

  int offense;
  int defense;
  int intimid;
  int persist;
  int dexterity;
  int discipline;

  float speed_multiplier = 1.0;
  float recovery = 1.0;
  float resilience = 1.0;
  int priority = 1;

  float acceleration = 0.0;
  float accel_rate = 5;
  float decel_rate = 10;

  std::unique_ptr<CombatAction> action;
  Status status;

  Color tint = WHITE;
  static constexpr float LOW_LIFE_THRESHOLD = 0.30;
protected:
  DamageType damage_type;
  StunType stun_type;

  float death_time = 1.0;
  float death_clock = 0.0;
private:
  float stun_time = 0;
  float stun_clock = 0.0;

  float knockback = 0;
  Direction kb_direction = LEFT;
  float kb_time = 0;
  float kb_clock = 0;

  Color start_tint;
};
