#pragma once
#include "data/keybinds.h"


namespace Input {
bool pressed(KeyBind &keybind, bool gamepad);
bool released(KeyBind &keybind, bool gamepad);
bool down(KeyBind &keybind, bool gamepad);
bool up(KeyBind &keybind, bool gamepad);
}
