#pragma once
#include <memory>
#include <vector>
#include "data/actor_event.h"

template <typename EventType>
using EventPool = std::vector<std::unique_ptr<EventType>>;


/* Serves the same purpose as FieldHandler with some notable difference.
 * This is designed to allow actors to somewhat communicate with 
 * eachother through queuing or responding to events. This class 
 * effectively functions as the middle-man to the whole process. Please
 * note that events could only be responded to by Actors, and this rule
 * must be keep to.*/
class ActorHandler {
public:
  ~ActorHandler();
  static EventPool<ActorEvent> *get();

  template<class Event, typename... Args>
  static void queue(Args... event_args) {
    event_queue.push_back(std::make_unique<Event>(Event{event_args...}));
  }

  static void clearEvents();
  static void transferEvents();
private:
  inline static EventPool<ActorEvent> event_queue;
  inline static EventPool<ActorEvent> event_pool;
};
