#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class StatusPanel : public Panel {
public:
  StatusPanel(SpriteAtlas *menu_atlas, MenuKeybinds *keybinds, 
              Session *session, std::string *description);
  ~StatusPanel();

  void update() override;
  void heightLerp();

  void optionNavigation();
  void draw() override;
private:
  Session *session;
  std::string *description;

  SpriteAtlas *menu_atlas;
  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {257, 48};

  std::array<PartyMemberID, 2> options {
    PartyMemberID::MARY
  };
std::array<PartyMemberID, 2>::iterator selected;
};
