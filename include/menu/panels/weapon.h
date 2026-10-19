#pragma once
#include <unordered_set>
#include <array>
#include <memory>
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

  void update() override;
  void optionNavigation();

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
};
