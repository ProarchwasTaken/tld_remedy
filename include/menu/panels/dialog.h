#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "data/keybinds.h"
#include "base/panel.h"


enum class DialogState {
  SCROLLING,
  END_OF_LINE,
  END_OF_DIALOG
};


class DialogPanel : public Panel {
public:
  DialogPanel(Vector2 position, std::vector<std::string> dialog);
  ~DialogPanel();

  void update() override;
  void heightLerp();
  void textScrolling();
  void confirm();

  void draw() override;
private:
  MenuKeybinds *keybinds;
  DialogState dialog_state = DialogState::SCROLLING;

  Vector2 main_position;
  Vector2 text_position;

  Texture texture;
  float frame_height;

  std::vector<std::string> dialog;
  std::vector<std::string>::iterator current_line;
  std::string::iterator current_char;
  std::string buffer;

  float text_speed = 0.10;
  float text_clock = 0.0;
};
