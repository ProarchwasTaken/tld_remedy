#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "data/session.h"
#include "system/sprite_atlas.h"


class SaveFile {
public:
  SaveFile(Vector2 position, SessionID file_id);
  ~SaveFile();

  void updateText(Session *data);
  void draw();
  void drawSaveInfo();

  static SpriteAtlas atlas;
private:
  bool valid;
  Vector2 main_position;
  Rectangle *frame;

  Color frame_color;
  Color pattern_color;
  Color gm_color;

  std::string location;
  std::string gamemode;
  std::string party;
  std::string playtime;
};
