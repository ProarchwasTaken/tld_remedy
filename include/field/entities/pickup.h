#pragma once
#include "enums.h"
#include "base/entity.h"
#include "data/entity.h"
#include "field/actors/player.h"


class Pickup : public Entity {
public:
  Pickup(PickupData &data);

  void update() override;
  void draw() override {};
private:
  PlayerActor *plr;
  bool in_range = false;

  PickupType pickup_type;
  int count;
};
