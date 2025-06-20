#pragma once
#include <raylib.h>


struct KeyBind {
  KeyboardKey key;
  GamepadButton button;
};

struct FieldKeybinds {
  KeyBind move_right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind move_left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};
  KeyBind move_down = {KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN};
  KeyBind move_up = {KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP};

  KeyBind interact = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
};

struct CombatKeybinds {
  KeyBind move_right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind move_left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};

  KeyBind attack = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
};
