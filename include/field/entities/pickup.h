#pragma once
#include "enums.h"
#include "base/entity.h"
#include "data/entity.h"
#include "system/sprite_atlas.h"
#include "field/actors/player.h"


class Pickup : public Entity {
public:
  Pickup(PickupData &data);
  ~Pickup();

  void interact();

  void update() override;
  void draw() override;
  static SpriteAtlas atlas;
private:
  int object_id;
  bool interacted = false;

  PlayerActor *plr;
  bool in_range = false;

  PickupType pickup_type;
  int count;
};
