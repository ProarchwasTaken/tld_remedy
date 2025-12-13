#pragma once
#include <string>
#include "enums.h"

struct Technique {
  std::string name;
  TechCostType type;
  float cost;

  float cooldown = 1.0;
  float clock = 1.0;
};
