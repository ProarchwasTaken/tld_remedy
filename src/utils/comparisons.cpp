#include <utility>
#include "base/combatant.h"
#include "utils/comparisons.h"

using std::pair;

bool Comparison::combatantDistance(const pair<float, Combatant *> &p1, 
                                   const pair<float, Combatant *> &p2) 
{
  int p1_priority = p1.second->priority;
  int p2_priority = p2.second->priority;

  if (p1_priority != p2_priority) {
    return p1_priority > p2_priority;
  }
  else {  
    return p1.first < p2.first;
  }
}
