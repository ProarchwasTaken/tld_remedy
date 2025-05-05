#include <raylib.h>
#include "data/keybinds.h"
#include "utils/input.h"


bool Input::pressed(KeyBind &keybind, bool gamepad) {
  bool key_input = IsKeyPressed(keybind.key);
  bool btn_input = gamepad && IsGamepadButtonPressed(0, keybind.button);

  return key_input || btn_input;
}

bool Input::released(KeyBind &keybind, bool gamepad) {
  bool key_input = IsKeyReleased(keybind.key);
  bool btn_input = gamepad && IsGamepadButtonReleased(0, keybind.button);

  return key_input || btn_input;
}
