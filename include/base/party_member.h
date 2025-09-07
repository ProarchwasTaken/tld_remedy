#pragma once
#include <string>
#include <raylib.h>
#include <set>
#include "enums.h"
#include "data/session.h"
#include "data/damage.h"
#include "base/combatant.h"


/* The PartyMember subclass is used by Combatants who are on the player's
 * side. Including the Combatant that's directly controlled by them.*/
class PartyMember : public Combatant {
public:
  static int memberCount() {return member_count;}
  PartyMember(std::string name, PartyMemberID id, Vector2 position);
  ~PartyMember();

  void takeDamage(DamageData &data) override;
  void finalIntercept(float &damage, DamageData &data) override;

  void damageLife(float magnitude) override;

  void afflictPersistent();
  void afflictPersistent(StatusID id);
  void afflictPersistent(StatusID status[STATUS_LIMIT]);
  std::set<StatusID> getEffectPool();
  StatusID selectRandomID(std::set<StatusID> &effect_pool);

  void damageMorale(float magnitude) override;
  void increaseMorale(float magnitude) override;

  void increaseExhaustion(float magnitude);
  void depleteExhaustion();
  void depleteInstant();

  void tintFlash();

  PartyMemberID id;
  bool important = false;
  bool demoralized = false;

  float morale;
  float init_morale;
  float max_morale;

  float exhaustion = 0.0;
  float deplete_delay = 0.5;
  float deplete_clock = 0.0;

  float recovery = 1.0;

  std::string tech1_name;
  float tech1_cost;
  TechCostType tech1_type;

  std::string tech2_name;
  float tech2_cost;
  TechCostType tech2_type;
private:
  static int member_count;
  constexpr static float DEFAULT_DEPLETE_DELAY = 0.5;
};
