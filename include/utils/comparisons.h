#pragma once
#include <utility>
#include "base/combatant.h"


namespace Comparison {
bool combatantDistance(const std::pair<float, Combatant*> &p1,
                       const std::pair<float, Combatant*> &p2);
}
