#pragma once
#include <array>
#include <cstddef>
#include <unordered_set>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class ItemsPanel : public Panel {
public:
  ItemsPanel(Session *session, std::string *description);
  ~ItemsPanel();

  void updateSelected();

  void update() override;
  void heightLerp();
  void optionNavigation();

  void draw() override;
  void drawItemCount();
  void drawOptions();
  std::string getShortenedName(ItemID item);
  void drawCursor(Vector2 position);
private:
  Session *session;
  std::string *description;

  SpriteAtlas *camp_atlas;
  SpriteAtlas *menu_atlas;

  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  Texture frame;
  float frame_height = 0;
  float percentage = 0;
  constexpr static Vector2 frame_position = {217, 42};

  std::array<ItemID, 8> options;
  std::array<ItemID, 8>::iterator selected = NULL;
  std::unordered_set<ItemID> disallowed = {ItemID::NONE};
  constexpr static Vector2 option_position = {41, 58};

  float blink_clock = 0.0;

  constexpr static Vector2 count_position = {107, 42};
};
