#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <raylib.h>
#include "data/keybinds.h"
#include "base/panel.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class DialogState {
  SCROLLING,
  END_OF_LINE,
  END_OF_DIALOG
};

enum class PromptOptions {
  YES,
  NO
};


class DialogPanel : public Panel {
public:
  DialogPanel(Vector2 position, std::vector<std::string> dialog, 
              bool end_prompt = false, bool visible_frame = true,
              float auto_time = -1);
  ~DialogPanel();

  void update() override;
  void heightLerp();
  void textScrolling();
  float getSpeedMultiplier();

  void menuNavigation(bool gamepad);
  void confirm();
  void autoConfirm();

  void draw() override;
  void drawOptions();
  void drawCursor(Vector2 position);

  std::array<PromptOptions, 2>::iterator selected = NULL;
private:
  MenuKeybinds *keybinds;
  SpriteAtlas *menu_atlas;
  SoundAtlas *sfx;

  DialogState dialog_state = DialogState::SCROLLING;
  std::array<PromptOptions, 2> prompt_options = {
    PromptOptions::YES,
    PromptOptions::NO
  };

  Vector2 main_position;
  Vector2 text_position;

  Texture texture;
  float frame_height;
  bool visible;

  std::vector<std::string> dialog;
  std::vector<std::string>::iterator current_line;
  std::string::iterator current_char;
  std::string buffer;

  float text_speed = 0.025;
  float text_clock = 0.0;
  float blink_clock = 0.0;

  bool auto_mode = false;
  float auto_time = -1;
  float auto_clock = 0.0;
};
