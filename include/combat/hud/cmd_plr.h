#pragma once
#include <string>
#include <raylib.h>
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class PlayerCmdHud {
public:
  PlayerCmdHud(Vector2 position);
  ~PlayerCmdHud();

  void assign(Mary *player);
  void draw();
  void drawNamePlate(Font *font, int txt_size);
private:
  Mary *player;
  SpriteAtlas *atlas;

  std::string player_name;

  Vector2 main_position;
  Vector2 name_position;
};
