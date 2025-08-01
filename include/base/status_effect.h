#pragma once
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"


class StatusEffect {
public:
  StatusEffect(StatusID id, StatusType type, Combatant *afflicted);
  virtual ~StatusEffect() {};

  virtual void init() {};
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
