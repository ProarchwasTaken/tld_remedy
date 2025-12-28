#pragma once
#include <string>
#include <raylib.h>
#include <memory>
#include <set>
#include "enums.h"
#include "data/session.h"
#include "data/damage.h"
#include "data/technique.h"
#include "data/combatant_event.h"
#include "base/combatant.h"
#include "system/sprite_atlas.h"


/* The PartyMember subclass is used by Combatants who are on the player's
 * side. Including the Combatant that's directly controlled by them.*/
class PartyMember : public Combatant {
public:
  static int memberCount() {return member_count;}
  PartyMember(std::string name, PartyMemberID id, Vector2 position,
              SpriteAtlas *atlas);
  ~PartyMember();

  bool isEnabled();
  virtual void setEnabled(bool value);

  void evaluateEvent(std::unique_ptr<CombatantEvent> &event) override;
  void moraleShare(GainedMoraleCBT *event);

  void takeDamage(DamageData &data) override;
  void finalIntercept(float &damage, DamageData &data) override;

  void damageLife(float magnitude) override;

  void afflictPersistent();
  void afflictPersistent(StatusID id, bool hide_text = false);
  void afflictPersistent(StatusID status[STATUS_LIMIT]);
  std::set<StatusID> getEffectPool();
  StatusID selectRandomID(std::set<StatusID> &effect_pool);

  void damageMorale(float magnitude) override;
  void increaseMorale(float magnitude, bool mp_share) override;

  void increaseExhaustion(float magnitude);
  void depleteExhaustion();
  void depleteInstant();

  void techniqueCooldown();

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

  Technique tech1;
  Technique tech2;
protected:
  bool enabled = true;
private:
  static int member_count;
  constexpr static float DEFAULT_DEPLETE_DELAY = 0.5;

  SpriteAtlas *atlas;
};
