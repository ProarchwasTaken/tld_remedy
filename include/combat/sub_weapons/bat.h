#pragma once
#include "base/sub_weapon.h"
#include "combat/combatants/party/mary.h"


class Bat : public SubWeapon {
public:
  Bat(Mary *user);
  ~Bat();
};
