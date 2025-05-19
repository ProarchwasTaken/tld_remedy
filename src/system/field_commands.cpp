#include <raylib.h>
#include <cassert>
#include "game.h"
#include "scenes/field.h"
#include "actors/player.h"
#include "system/field_commands.h"
#include <plog/Log.h>

FieldScene *CommandSystem::scene;

CommandSystem::CommandSystem(FieldScene *scene) {
  assert(Game::devmode);
  this->scene = scene;
  PLOGD << "Initialized debug command system.";
}

CommandSystem::~CommandSystem() {
  PLOGD << "Reseting debug command system.";
}

void CommandSystem::process() {
  if (IsKeyPressed(KEY_SLASH)) {
    toggleCommandMode();
  }
}

void CommandSystem::toggleCommandMode() {
  command_mode = !command_mode;
  if (command_mode) {
    PLOGD << "Entering command mode.";
  }
  else {
    PLOGD << "Exiting command mode.";
  }

  PlayerActor::setControllable(!command_mode);
}


