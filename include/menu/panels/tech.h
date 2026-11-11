#pragma once
#include <array>
#include <memory>
#include <string>
#include <unordered_set>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "menu/panels/text_box.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"

enum class TechCategory {
  NONE = -1,
  SUB_WEAPON,
  ASSIST,
  ACTION,
  PASSIVE
};

enum class TechOption {
  SLOT1,
  SLOT2,
  SLOT3,
  SLOT4,
  SLOT5,
  SLOT6
};

struct TechInfo {
  TechCategory category;
  std::string name;
  std::string type;
  std::string desc;
};


class TechsPanel : public Panel {
public:
  TechsPanel(Session *session, std::string *description, 
             Color *desc_color);
  ~TechsPanel();

  void setupTech();
  void setupWeaponTech(SubWeaponID weapon_id, 
                       std::array<TechInfo, 6>::iterator &tech);

  std::string getCategoryName(TechCategory category);
  Color getPrimaryColor(TechCategory category);
  Color getSecondaryColor(TechCategory category);

  void update() override;
  void heightLerp();

  void memberNavigation();
  void techNavigation();
  void createTextbox();

  void draw() override;

  void drawOptions();
  void drawCursor(Vector2 position, bool blink);
  void drawTech();
  void drawTechCategory(TechCategory category, Vector2 position, 
                        Color frame_color);
private:
  SpriteAtlas *camp_atlas;
  SpriteAtlas *menu_atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  Font *med_font;
  int med_size;

  std::string *description;
  Color *desc_color;

  float percentage = 0;

  constexpr static Vector2 option_position = {41, 58};
  std::array<Character*, 2> options;
  std::array<Character*, 2>::iterator selected;

  constexpr static Vector2 tech_position = {257, 42};
  std::array<TechOption, 6> tech_options = {
    TechOption::SLOT1,
    TechOption::SLOT2,
    TechOption::SLOT3,
    TechOption::SLOT4,
    TechOption::SLOT5,
    TechOption::SLOT6
  };

  std::array<TechOption, 6>::iterator tech_selected;
  std::unordered_set<TechOption> tech_disallowed;

  std::array<TechInfo, 6> tech_info;
  float blink_clock = 0.0;

  enum {
    PARTY_MEMBER,
    TECH_SELECT,
    PANEL
  } menu_state = PARTY_MEMBER;

  std::unique_ptr<TextBoxPanel> panel;
};
