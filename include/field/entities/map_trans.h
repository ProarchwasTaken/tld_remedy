#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/entity.h"
#include "data/entity.h"
#include "data/session.h"
#include "field/actors/player.h"


class MapTransition : public Entity {
public:
  MapTransition(Session *session, MapTransData &data);

  void update() override;
  void draw() override {};
private:
  PlayerActor *plr;
  Session *session;

  std::string map_dest;
  std::string spawn_dest;
  Direction direction;
  FlagID required_flag;

  bool in_trigger = false;
};
