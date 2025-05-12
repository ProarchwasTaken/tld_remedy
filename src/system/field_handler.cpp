#include <memory>
#include <plog/Log.h>
#include "data/field_event.h"
#include "system/field_handler.h"


FieldEventHandler::~FieldEventHandler() {
  PLOGI << "Clearing leftover events from memory.";
  for (auto &event : event_pool) {
    event.reset();
  }

  event_pool.clear();
}

EventPool<FieldEvent> *FieldEventHandler::get() {
  return &event_pool;
}
