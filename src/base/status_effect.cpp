#include "enums.h"
#include "base/combatant.h"
#include "base/status_effect.h"


StatusEffect::StatusEffect(StatusID id, StatusType type, 
                           Combatant *afflicted) 
{
  this->id = id;
  this->type = type;
  this->afflicted = afflicted;
}
