#pragma once
#include <unordered_set>
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
#include "menu/hud/reticle.h"
#include "menu/panels/dialog.h"


class FileSelectPanel : public Panel {
public:
  FileSelectPanel(Session *session = NULL);
  ~FileSelectPanel();

  void setupSaveFiles();
  void setupMainText();
  void updateSelected();

  void update() override;
  void panelLogic();
  void promptHandling(PromptOptions response);

  void menuNavigation();
  void openDialog();
  void saveDelay();

  void draw() override;
private:
  Session *session;
  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  std::string text;
  Vector2 text_position;
  Rectangle text_rect;

  bool save_mode;
  bool load_game = false;

  bool saved_game = false;
  float delay_time = 1.0;
  float delay_clock = 0.0;

  std::array<SessionID, 3> options = {
    SessionID::FILE1,
    SessionID::FILE2,
    SessionID::FILE3
  };
  std::array<SessionID, 3>::iterator selected;
  std::array<std::unique_ptr<SaveFile>, 3> save_files;
  std::unordered_set<SessionID> disallowed;

  TargetReticle reticle = TargetReticle({210, 50});
  float blink_clock = 0;

  std::unique_ptr<DialogPanel> panel = nullptr;
};
