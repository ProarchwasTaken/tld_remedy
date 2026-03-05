#pragma once
#include "enums.h"
#include "data/session.h"


namespace Flag {
bool check(Session *session, FlagID id);
void set(Session *session, FlagID id, bool value);
}
