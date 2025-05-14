#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/entity.h"
#include "actors/player.h"


class MapTransition : public Entity {
public:
  MapTransition(std::string map_dest, std::string spawn_dest,
                Rectangle rect, enum Direction direction);

  void update() override;
  void draw() override {};
private:
  PlayerActor *plr;

  std::string map_dest;
  std::string spawn_dest;
  Direction direction;
};
