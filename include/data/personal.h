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

  bool operator!=(const Settings &cfg) {
    return this->master_volume != cfg.master_volume ||
    this->sfx_volume != cfg.sfx_volume ||
    this->bgm_volume != cfg.bgm_volume ||
    this->fullscreen != cfg.fullscreen ||
    this->framerate != cfg.framerate; 
  }
};

struct Personal {
  unsigned int version;
  Settings settings;
};
