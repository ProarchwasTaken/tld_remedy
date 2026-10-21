#pragma once
#include <unordered_set>
#include <array>
#include <memory>
#include <string>
#include <cstddef>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "menu/hud/w_portrait.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class WeaponOption {
  BIRTHRIGHT,
  HEROISM,
  BRAVERY,
  DEFIANCE,
  SPECIALIST
};


class WeaponPanel : public Panel {
public:
  WeaponPanel();
  ~WeaponPanel();

  void initPortraits();
  void initWeaponText();

  void update() override;
  void optionNavigation();
  void textScroll();

  void startOptionMotion();
  void optionMotion();
  void selectedLogic(WeaponPortrait *portrait);
  void prevSelectedLogic(WeaponPortrait *portrait);

  void draw() override;

  std::array<WeaponOption, 5>::iterator selected = NULL;
private:
  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  Texture gradient;

  std::array<WeaponOption, 5> options = {
    WeaponOption::DEFIANCE,
    WeaponOption::BRAVERY,
    WeaponOption::BIRTHRIGHT,
    WeaponOption::HEROISM,
    WeaponOption::SPECIALIST
  };
  std::array<WeaponOption, 5>::iterator prev_selected;
  std::unordered_set<WeaponOption> disallowed;

  std::array<std::unique_ptr<WeaponPortrait>, 5> portraits;
  Vector2 option_position = {213, 120};

  float opt_spacing = 88;
  float opt_clock = 1.0;
  float opt_time = 0.30;
  float opt_start_x;
  float opt_end_x;

  Font *sm_font;
  int sm_size;

  Font *med_font;
  int med_size;

  std::string name;
  std::string name_buffer;
  std::string::iterator name_iter;
  Vector2 name_position;

  std::string type;
  std::string type_buffer;
  std::string::iterator type_iter;
  Vector2 type_position;

  std::string blurb;
  std::string blurb_buffer;
  std::string::iterator blurb_iter;
  Vector2 blurb_position;

  float scroll_clock = 1.0;
  float scroll_time = 0.20;
};
