#pragma once
#include <raylib.h>


struct KeyBind {
  KeyboardKey key;
  GamepadButton button;

  bool operator!=(const KeyBind &keybind) {
    return this->key != keybind.key || this->button != keybind.button;
  }
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

  bool operator!=(const FieldKeybinds &keybinds) {
    return this->move_right != keybinds.move_right ||
    this->move_left != keybinds.move_left ||
    this->move_down != keybinds.move_down ||
    this->move_up != keybinds.move_up ||
    this->interact != keybinds.interact ||
    this->open_menu != keybinds.open_menu;
  }
};

struct CombatKeybinds {
  KeyBind move_right = {KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT};
  KeyBind move_left = {KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT};
  KeyBind down = {KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN};

  KeyBind attack = {KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN};
  KeyBind defensive = {KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT};
  KeyBind light_tech = {KEY_A, GAMEPAD_BUTTON_RIGHT_FACE_LEFT};
  KeyBind heavy_tech = {KEY_S, GAMEPAD_BUTTON_RIGHT_FACE_UP};
  KeyBind light_assist = {KEY_D, GAMEPAD_BUTTON_RIGHT_TRIGGER_1};
  KeyBind heavy_assist = {KEY_C, GAMEPAD_BUTTON_RIGHT_TRIGGER_2};

  KeyBind pause = {KEY_TAB, GAMEPAD_BUTTON_MIDDLE_RIGHT};
  KeyBind use_item = {KEY_LEFT_SHIFT, GAMEPAD_BUTTON_MIDDLE_LEFT};

  bool operator!=(const CombatKeybinds &keybinds) {
    return this->move_right != keybinds.move_right ||
    this->move_left != keybinds.move_left ||
    this->down != keybinds.down ||
    this->attack != keybinds.attack ||
    this->defensive != keybinds.defensive ||
    this->light_tech != keybinds.light_tech ||
    this->heavy_tech != keybinds.heavy_tech ||
    this->light_assist != keybinds.light_assist ||
    this->heavy_assist != keybinds.heavy_assist ||
    this->pause != keybinds.pause ||
    this->use_item != keybinds.use_item;
  }
};
