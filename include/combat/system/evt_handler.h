#pragma once
#include <memory>
#include <vector>
#include "data/combat_event.h"

template <typename EventType>
using EventPool = std::vector<std::unique_ptr<EventType>>;


class CombatHandler {
public:
  ~CombatHandler();
  static EventPool<CombatEvent> *get();

  template<class Event, typename... Args>
  static void raise(Args... event_args) {
    event_pool.push_back(std::make_unique<Event>(Event{event_args...}));
  }

  static void clear();
private:
  inline static EventPool<CombatEvent> event_pool;
};
