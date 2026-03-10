#pragma once
#include "enums.h"
#include "base/entity.h"
#include "data/entity.h"
#include "data/session.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "field/actors/player.h"


class Pickup : public Entity {
public:
  Pickup(Session *session, PickupData &data);
  ~Pickup();

  void interact();
  void openFlagDialog(FlagID flag);

  void update() override;
  void draw() override;
  static SpriteAtlas atlas;
private:
  int object_id;
  bool interacted = false;

  PlayerActor *plr;
  bool in_range = false;

  Session *session;
  PickupType pickup_type;
  int value;

  Animation anim_idle = {{0, 1, 2}, 0.10};
};
