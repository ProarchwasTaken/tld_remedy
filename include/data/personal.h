#pragma once
#include "data/keybinds.h"


struct Settings {
  float master_volume = 1.0;
  float sfx_volume = 1.0;
  float bgm_volume = 1.0;

  bool fullscreen = false;
  int framerate = 60;

  MenuKeybinds menu_keybinds;
  FieldKeybinds field_keybinds;
  CombatKeybinds combat_keybinds;
};

struct Personal {
  unsigned int version;
  Settings settings;
};
