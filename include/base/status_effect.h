#pragma once
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"


enum class StatusType {
  POSITIVE,
  NEGATIVE
};


/* Status Effects are conditions that temporarily modifies a Combatant's
 * usual attributes or behavior. They can either be positive, or 
 * negative depending on the status effect.*/
class StatusEffect {
public:
  StatusEffect(StatusID id, StatusType type, Combatant *afflicted);
  virtual ~StatusEffect();

  virtual void init(bool hide_text = false);
  bool isPersistant() {return persistant;}

  virtual void intercept(DamageData &data) {}
  virtual void logic() {}

  StatusID id;
  StatusType type;
  std::string name;

  bool end = false;
protected:
  bool persistant = false;
  Combatant *afflicted;
};
