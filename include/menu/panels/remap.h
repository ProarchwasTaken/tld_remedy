#pragma once
#include <unordered_set>
#include <array>
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/personal.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class RemapOption {
  INPUT_RIGHT,
  INPUT_LEFT,
  INPUT_DOWN,
  INPUT_UP,
  INPUT_A,
  INPUT_B,
  INPUT_X,
  INPUT_Y,
  INPUT_L,
  INPUT_R,
  INPUT_START,
  INPUT_SELECT
};

enum class InputCategory {
  FIELD,
  COMBAT
};


class RemapPanel : public Panel {
public:
  RemapPanel(SpriteAtlas *menu_atlas, Settings *settings);
  ~RemapPanel();

  void updateDisallowed();
  void updateSelected();

  KeyBind *retrieveFieldKeybind(RemapOption option);
  KeyBind *retrieveCombatKeyBind(RemapOption option);

  void update() override;
  void heightLerp();
  void optionNavigation();
  void inputPrompting();

  void draw() override;
  void drawCategory(Font *font, int txt_size);
  void drawOptions(Font *font, int txt_size);
  void drawCursor(Vector2 position);
  void drawRedBar(Vector2 position);

  void drawKeybind(RemapOption &option, Vector2 position, Font *font,
                        int txt_size);
  std::string getKeyName(KeyboardKey key);
  std::string getButtonName(GamepadButton button);

  std::string getFieldKeybindName(RemapOption option);
  std::string getCombatKeyBindName(RemapOption option);
private:
  Texture frame;
  const Vector2 main_position = {60, 4};
  float frame_height = 0;

  std::array<InputCategory, 2> category = {
    InputCategory::FIELD,
    InputCategory::COMBAT
  };
  std::array<InputCategory, 2>::iterator current_category = NULL;

  std::array<RemapOption, 12> options = {
    RemapOption::INPUT_RIGHT,
    RemapOption::INPUT_LEFT,
    RemapOption::INPUT_DOWN,
    RemapOption::INPUT_UP,
    RemapOption::INPUT_A,
    RemapOption::INPUT_B,
    RemapOption::INPUT_X,
    RemapOption::INPUT_Y,
    RemapOption::INPUT_L,
    RemapOption::INPUT_R,
    RemapOption::INPUT_START,
    RemapOption::INPUT_SELECT
  };
  std::array<RemapOption, 12>::iterator selected;
  std::unordered_set<RemapOption> disallowed;
  float blink_clock = 0;

  bool input_mode = false;
  float gamepad_delay = 0.25;
  float delay_clock = 0.0;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
  Settings *settings;
};
