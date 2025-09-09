#include <cassert>
#include <memory>
#include "data/combatant_event.h"
#include "combat/system/cbt_handler.h"


CombatantHandler::~CombatantHandler() {
  for (auto &event : event_queue) {
    event.reset();
  }

  event_queue.clear();
  CombatantHandler::clearEvents();
}

EventPool<CombatantEvent> *CombatantHandler::get() {
  return &event_pool;
}

void CombatantHandler::clearEvents() {
  for (auto &event : event_pool) {
    event.reset();
  }

  event_pool.clear();
}

void CombatantHandler::transferEvents() {
  int count = event_queue.size();
  if (count == 0) {
    return;
  }

  assert(event_pool.empty());
  event_queue.swap(event_pool);
}
