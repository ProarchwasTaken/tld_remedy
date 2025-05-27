#include <memory>
#include <plog/Log.h>
#include "data/field_event.h"
#include "field/system/field_handler.h"


FieldHandler::~FieldHandler() {
  FieldHandler::clear();
}

EventPool<FieldEvent> *FieldHandler::get() {
  return &event_pool;
}

void FieldHandler::clear() {
  for (auto &event : event_pool) {
    event.reset();
  }

  event_pool.clear();
}
