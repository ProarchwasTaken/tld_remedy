#pragma once
#include <string>
#include "enums.h"


namespace ItemUtils {
std::string getName(ItemID item);
std::string getShortened(ItemID item);
std::string getDescription(ItemID item);
}
