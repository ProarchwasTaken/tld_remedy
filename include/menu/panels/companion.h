#pragma once
#include <unordered_set>
#include <memory>
#include <array>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "menu/hud/portrait.h"
#include "menu/hud/archetype.h"
#include "menu/hud/stat_sheet.h"
#include "menu/hud/rating.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class CompanionPanel : public Panel {
public:
  CompanionPanel();
  ~CompanionPanel();

  void initPortraits();
  void setupCompanionData();

  void update() override;
  void transitionLerp();

  void readyLogic();
  void optionNavigation();
  void textScroll();

  void startOptionMotion();
  void optionMotion();
  void selectedLogic(Portrait *portrait);
  void prevSelectedLogic(Portrait *portrait);

  void informationLerp();
  void confirmInput();

  void draw() override;

  std::array<CompanionID, 2>::iterator selected;
  bool exiting_out = false;
private:
  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  enum {
    SELECTION,
    INFO_IN,
    INFORMATION,
    INFO_OUT,
  } menu_state = SELECTION;

  float menu_clock = 0.0;
  float menu_time = 0.6;

  Texture info_frame1;
  Texture info_frame2;
  Rectangle info_source = {0, 0, 161, 0};

  Rectangle title_sprite;
  Vector2 title_position = {16, 16};

  std::array<CompanionID, 2> options = {
    CompanionID::XANDER,
    CompanionID::ERWIN
  };
  std::array<CompanionID, 2>::iterator prev_selected;
  std::unordered_set<CompanionID> disallowed;

  std::array<std::unique_ptr<Portrait>, 2> portraits;
  Vector2 option_position = {181, 48};

  float opt_spacing = 86;
  float opt_clock = 1.0;
  float opt_time = 0.20;
  float opt_start_x;
  float opt_end_x;

  Font *sm_font;
  int sm_size;

  Font *med_font;
  int med_size;

  std::string text;
  std::string text_buffer;
  std::string::iterator text_iter;
  Vector2 text_position;

  float scroll_clock = 1.0;
  float scroll_time = 0.30;

  std::unique_ptr<Archetype> icon;
  std::unique_ptr<StatSheet> stats;

  StarRating influence = StarRating({291, 69}, WHITE);
  StarRating independence = StarRating({291, 85}, WHITE);
  StarRating sustainability = StarRating({291, 101}, WHITE);

  std::string name;
  std::string archetype;
  Vector2 arch_position;

  std::string description;
  Color main_color;
};
