#include <cassert>
#include "enums.h"
#include "data/session.h"
#include "utils/flag.h"
#include <plog/Log.h>


bool Flag::check(Session *session, FlagID id) {
  assert(id != FlagID::NONE);
  for (int x = 0; x < FLAG_COUNT; x++) {
    GameFlag *flag = &session->flags[x];

    if (flag->id == id) {
      PLOGD << "Returning value of Flag ID: " << static_cast<int>(id);
      return flag->raised;
    }
  }

  return false;
}

void Flag::set(Session *session, FlagID id, bool value) {
  assert(id != FlagID::NONE);
  for (int x = 0; x < FLAG_COUNT; x++) {
    GameFlag *flag = &session->flags[x];

    if (flag->id == id) {
      PLOGD << "Flag of ID: " << static_cast<int>(id) << 
        " has been set to: " << value;
      flag->raised = value;
      return;
    }
  }

  PLOGE << "Invalid Flag!";
}
