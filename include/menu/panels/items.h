#pragma once
#include <memory>
#include <array>
#include <vector>
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
#include "menu/panels/dialog.h"
#include "menu/hud/reticle.h"


class ItemsPanel : public Panel {
public:
  ItemsPanel(Session *session, std::string *description, 
             Color *desc_color);
  ~ItemsPanel();

  void updateSelected();

  void useItem();
  float calculateHeal(Character *member, float percentage);
  void openDialog(std::vector<std::string> &dialog);
  void openRejectDialog(Character *member); 
  void openHealDialog(Character *member, float healed);
  void openSplintDialog(Character *member, StatusID effect);

  void update() override;
  void panelLogic();
  void heightLerp();
  void optionNavigation();
  void targetNavigation();

  void draw() override;
  void drawItemCount();

  void drawOptions();
  void drawCursor(Vector2 position);

  void drawItemInfo();
  void drawItemName(Font *font, int txt_size);
  void drawItemType(Font *font, int txt_size);
  void drawItemUsable(Font *font, int txt_size);
  void drawItemDesc(Font *font, int txt_size);
  void drawItemPortrait();

  static SpriteAtlas portraits;
private:
  Session *session;
  std::string *description;
  Color *desc_color;

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
  std::string item_name;
  float blink_clock = 0.0;

  bool target_mode = false;
  std::array<Character*, 2> party;
  std::array<Character*, 2>::iterator target;
  TargetReticle reticle;

  constexpr static Vector2 count_position = {107, 42};

  std::unique_ptr<DialogPanel> panel;
  bool panel_mode = false;
};
