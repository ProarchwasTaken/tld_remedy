#pragma once
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/hud/savefile.h"


class FileSelectPanel : public Panel {
public:
  FileSelectPanel(Session *session = NULL);
  ~FileSelectPanel();

  void setupSaveFiles();

  void update() override;
  void draw() override;

private:
  std::string text;
  Vector2 text_position;
  bool save_mode;

  std::array<SessionID, 3> options = {
    SessionID::FILE1,
    SessionID::FILE2,
    SessionID::FILE3
  };
  std::array<SessionID, 3>::iterator selected;
  std::array<std::unique_ptr<SaveFile>, 3> save_files;

  Session *session;
  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
