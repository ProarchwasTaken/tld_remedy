#pragma once
#include <raylib.h>


struct KeyBind {
  KeyboardKey key;
  GamepadButton button;
};

struct MenuKeybinds {
  KeyBind right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};
  KeyBind down = {KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN};
  KeyBind up = {KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP};

  KeyBind confirm = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
  KeyBind cancel = {KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT};
};

struct FieldKeybinds {
  KeyBind move_right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind move_left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};
  KeyBind move_down = {KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN};
  KeyBind move_up = {KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP};

  KeyBind interact = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
  KeyBind open_menu = {KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT};
};

struct CombatKeybinds {
  KeyBind move_right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind move_left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};
  KeyBind down = {KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN};

  KeyBind attack = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
  KeyBind defensive = {KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT};
  KeyBind light_tech = {KEY_A, GAMEPAD_BUTTON_RIGHT_FACE_LEFT};
  KeyBind heavy_tech = {KEY_S, GAMEPAD_BUTTON_RIGHT_FACE_UP};
};
