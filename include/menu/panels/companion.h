#pragma once
#include <unordered_set>
#include <memory>
#include <array>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "menu/hud/portrait.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class CompanionPanel : public Panel {
public:
  CompanionPanel();
  ~CompanionPanel();

  void initPortraits();

  void update() override;
  void transitionLerp();

  void readyLogic();
  void optionNavigation();

  void startOptionMotion();
  void optionMotion();
  void selectedLogic(Portrait *portrait);
  void prevSelectedLogic(Portrait *portrait);

  void draw() override;

  std::array<CompanionID, 2>::iterator selected;
  bool exiting_out = false;
private:
  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

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
};
