#pragma once
#include <string>
#include "enums.h"
#include "data/damage.h"
#include "base/combatant.h"


/* Actions are something that every combatant can perform. Think of them 
 * as special behavior that the Combatant executes under certain 
 * conditions After which, the Combatant will be locked into using said 
 * special behavior until the Action ends, or they get interrupted.*/
class CombatAction {
public:
  CombatAction(ActionID id, ActionType type, Combatant *user, 
               float wind_up, float action, float end_lag);
  virtual ~CombatAction();

  virtual void intercept(DamageData &data);
  void logic();
  void proceed();

  virtual void windUp() = 0;
  virtual void action() = 0;
  virtual void endLag() = 0;

  virtual void drawDebug() {};

  ActionID id;
  ActionPhase phase;
  ActionType type;
  std::string name;

  float wind_time;
  float act_time;
  float end_time;

  float state_clock = 0.0;
private:
  Combatant *user;
};
