#pragma once
#include <array>
#include <cstddef>
#include <unordered_set>
#include <raylib.h>
#include "enums.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/combatants/party/mary.h"


class ItemCmdHud {
public:
  ItemCmdHud(Vector2 position);
  ~ItemCmdHud();

  void assign(Mary *&player, PartyMember *&companion, Session *session);

  void enable();
  void updateSelected();

  void disable();

  void update();
  void optionNavigation();

  void draw();
  void drawNamePlate(Font *font, int txt_size);
  void drawOptions(Font *font, int txt_size);
  void drawOptionText(ItemID item, Vector2 position, Font *font, 
                      int txt_size);

  bool enabled = false;
private:
  Mary **player = NULL;
  PartyMember **companion = NULL;

  Session *session;

  Vector2 main_position;
  static float base_y;
  Color main_color;

  std::array<ItemID, 8> options;
  std::array<ItemID, 8>::iterator selected = NULL;
  std::unordered_set<ItemID> disallowed = {ItemID::NONE};

  float opt_switch_clock = 0.0;
  float opt_switch_time = 0.15;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
