#pragma once
#include <string>
#include <raylib.h>
#include <set>
#include "enums.h"
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
  std::set<StatusID> getEffectPool();
  StatusID selectRandomID(std::set<StatusID> &effect_pool);

  void damageMorale(float magnitude) override;
  void increaseMorale(float magnitude) override;

  void increaseExhaustion(float magnitude);
  void depleteExhaustion();
  void depleteInstant();

  PartyMemberID id;
  bool important = false;

  float morale;
  float init_morale;
  float max_morale;

  float recovery = 1.0;

  float exhaustion = 0.0;
  float deplete_delay = 0.5;
  float deplete_clock = 0.0;
private:
  static int member_count;
  constexpr static float DEFAULT_DEPLETE_DELAY = 0.5;
};
