#pragma once
#include <memory>
#include <vector>
#include "data/combatant_event.h"

template <typename EventType>
using EventPool = std::vector<std::unique_ptr<EventType>>;


/* Designed to allow Combatants to somewhat communicate with eachother
 * through queuing and responding to events. The class effectively
 * functions as the middle-man to the whole process. While technically,
 * any entity can queue events, only they could be acknowledged and
 * responded to by Combatants. Barring some notable exceptions, this
 * rule must be kept to.*/
class CombatantHandler {
public:
  ~CombatantHandler();
  static EventPool<CombatantEvent> *get();

  template<class Event, typename... Args>
  static void queue(Args... event_args) {
    event_queue.push_back(std::make_unique<Event>(Event{event_args...}));
  }

  static void clearEvents();
  static void transferEvents();
private:
  inline static EventPool<CombatantEvent> event_queue;
  inline static EventPool<CombatantEvent> event_pool;
};
