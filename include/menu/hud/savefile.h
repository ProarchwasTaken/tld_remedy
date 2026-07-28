#pragma once
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/session.h"
#include "system/sprite_atlas.h"


class SaveFile {
public:
  SaveFile(Vector2 position, SessionID file_id);
  ~SaveFile();

  void updateText(Session *data, bool active_playtime);
  void update(PanelState state, float clock);

  void draw();
  void drawSaveInfo();

  SessionID file_id;
  Vector2 main_position;
  bool valid;

  static SpriteAtlas atlas;
private:
  Rectangle frame;
  float frame_height = 42;

  Color frame_color;
  Color pattern_color;
  Color gm_color;

  std::string location;
  std::string gamemode;
  std::string party;
  std::string playtime;
};
