#pragma once
#include <memory>
#include <vector>
#include "data/field_event.h"

template <typename EventType>
using EventPool = std::vector<std::unique_ptr<EventType>>;


/* Designed to allow the entities and actors to communicate with the
 * Field scene via raising events. From there, it's up to the scene to
 * decide what to do.*/
class FieldEventHandler {
public:
  ~FieldEventHandler();
  static EventPool<FieldEvent> *get();

  template<class Event, typename... Args>
  static void raise(Args... event_args) {
    event_pool.push_back(std::make_unique<Event>(Event{event_args...}));
  }

  static void clear();
private:
  inline static EventPool<FieldEvent> event_pool;
};
