#pragma once
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class ItemsPanel : public Panel {
public:
  ItemsPanel(Session *session, std::string *description);
  ~ItemsPanel();

  void update() override;
  void heightLerp();
  void optionNavigation();

  void draw() override;
private:
  Session *session;
  std::string *description;

  SpriteAtlas *camp_atlas;
  SpriteAtlas *menu_atlas;

  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {217, 42};
};
