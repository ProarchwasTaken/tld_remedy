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
  void selectOption();
  bool enterTargetMode();

  void targetNavigation();
  void useItem();

  void draw();
  void drawNamePlate(const char *text, Font *font, int txt_size, 
                     Vector2 position, Color main_color, 
                     Color col_pattern);
  void drawOptions(Font *font, int txt_size);
  void drawOptionText(ItemID item, Vector2 position, Font *font, 
                      int txt_size);
  void drawMemberName(PartyMember *member, Vector2 position, Font *font, 
                      int txt_size);

  void drawTargetOptions(Font *font, int txt_size);

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

  Vector2 target_position;
  Color target_color;
  bool target_mode = false;

  std::array<PartyMember*, 2> party;
  std::array<PartyMember*, 2>::iterator target = NULL;

  float opt_switch_clock = 0.0;
  float opt_switch_time = 0.15;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
