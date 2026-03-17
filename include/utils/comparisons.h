#pragma once
#include <utility>
#include "enums.h"
#include "base/combatant.h"


namespace Comparison {
bool combatantDistance(const std::pair<float, Combatant*> &p1,
                       const std::pair<float, Combatant*> &p2);
bool combatantPriority(const std::pair<float, Combatant*> &p1,
                       const std::pair<float, Combatant*> &p2);
bool effectAlgorithm(const StatusID effect1, const StatusID effect2);
}
