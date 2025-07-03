#include <memory>
#include "data/combat_event.h"
#include "combat/system/evt_handler.h"


CombatHandler::~CombatHandler() {
  CombatHandler::clear();
}

EventPool<CombatEvent> *CombatHandler::get() {
  return &event_pool;
}

void CombatHandler::clear() {
  for (auto &event : event_pool) {
    event.reset();
  }
  
  event_pool.clear();
}
