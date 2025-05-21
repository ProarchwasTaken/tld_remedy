#pragma once 
#include <string>
#include "enums.h"


struct FieldCommand {
  CommandType type;
};

struct ChangeMapCommand : FieldCommand {
  std::string map_name;
};

struct SetSuppliesCommand : FieldCommand {
  int value;
};
