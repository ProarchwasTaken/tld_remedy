#include <cassert>
#include <memory>
#include "data/actor_event.h"
#include "field/system/actor_handler.h"
#include <plog/Log.h>


ActorHandler::~ActorHandler() {
  for (auto &event : event_queue) {
    event.reset();
  }

  event_queue.clear();

  ActorHandler::clearEvents();
}

EventPool<ActorEvent> *ActorHandler::get() {
  return &event_pool;
}

void ActorHandler::clearEvents() {
  for (auto &event : event_pool) {
    event.reset();
  }

  event_pool.clear(); 
}

void ActorHandler::transferEvents() {
  int count = event_queue.size();
  if (count == 0) {
    return;
  }

  assert(event_pool.empty());
  event_queue.swap(event_pool);
}
