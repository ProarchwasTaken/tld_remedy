#pragma once
#include <array>
#include <string>
#include <memory>
#include <unordered_set>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/personal.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/panels/confirm.h"


enum class ConfigOption {
  VOL_MASTER,
  VOL_SOUND,
  VOL_MUSIC,
  FULLSCREEN,
  FRAMERATE,
  CONTROLS,
  APPLY,
  BACK
};


class ConfigPanel : public Panel {
public:
  ConfigPanel(SpriteAtlas *menu_atlas, MenuKeybinds *keybinds);
  ~ConfigPanel();

  void update() override;
  void heightLerp();
  void terminateConfirmPanel();

  void verticalNavigation(bool gamepad);
  void horizontalInput(bool gamepad);
  bool changeVolume(int direction, float *percentage);
  bool changeFramerate(int direction);

  void selectOption();
  void applySettings();
  void closePanel();

  void draw() override;
  void drawOptions();
  void drawOptionVisuals(ConfigOption id, Vector2 position, Font *font, 
                         int txt_size);
  void drawGuage(Vector2 position, float percentage);
  void drawCheckbox(Vector2 position, bool enabled);
  void drawFramerateStepper(Vector2 position, Font *font, int txt_size);
  void drawCursor(Vector2 position);

  std::string getOptionName(ConfigOption id);
private:
  Texture frame;
  Vector2 position = {97, 39};

  float frame_height = 161;
  float blink_clock = 0;

  std::array<ConfigOption, 8> options {
    ConfigOption::VOL_MASTER,
    ConfigOption::VOL_SOUND,
    ConfigOption::VOL_MUSIC,
    ConfigOption::FULLSCREEN,
    ConfigOption::FRAMERATE,
    ConfigOption::CONTROLS,
    ConfigOption::APPLY,
    ConfigOption::BACK
  };
  std::array<float, 8> y_values = {55, 71, 87, 111, 127, 151, 167, 183};

  std::array<ConfigOption, 8>::iterator selected;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;

  Settings settings;
  bool unapplied = false;

  std::unique_ptr<ConfirmPanel> panel;
  bool panel_mode = false;

  bool on_linux;
  std::unordered_set<ConfigOption> disallowed;
};
