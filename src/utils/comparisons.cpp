#include <utility>
#include "base/combatant.h"
#include "utils/comparisons.h"

using std::pair;

bool Comparison::combatantDistance(const pair<float, Combatant *> &p1, 
                                   const pair<float, Combatant *> &p2) 
{
  return p1.first < p2.first;
}
