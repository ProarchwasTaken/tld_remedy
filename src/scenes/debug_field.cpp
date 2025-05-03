#include <raylib.h>
#include <plog/Log.h>
#include "scenes/debug_field.h"

DebugField::DebugField() {
  PLOGI << "Initialized the DebugField Scene.";
}

void DebugField::update() {

}

void DebugField::draw() {
  DrawRectangle(0, 0, 32, 32, BLACK);
}
 
