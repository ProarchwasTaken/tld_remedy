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

bool Input::down(KeyBind &keybind, bool gamepad) {
  bool key_input = IsKeyDown(keybind.key);
  bool btn_input = gamepad && IsGamepadButtonDown(0, keybind.button);

  return key_input || btn_input;
}

bool Input::up(KeyBind &keybind, bool gamepad) {
  bool key_input = IsKeyUp(keybind.key);
  bool btn_input = gamepad && IsGamepadButtonUp(0, keybind.button);

  return key_input || btn_input;
}
