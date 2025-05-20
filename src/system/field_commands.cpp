#include <raylib.h>
#include <string>
#include <cctype>
#include <cassert>
#include "enums.h"
#include "data/field_event.h"
#include "game.h"
#include "scenes/field.h"
#include "system/field_handler.h"
#include "actors/player.h"
#include "system/field_commands.h"
#include <plog/Log.h>

using std::string;
string findNextWord(string &buffer, string::iterator &iterator, 
                    bool uppercase = false);
void loadMapCommand(string map_name, string spawn_name);


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
    return;
  }

  if (!command_mode) {
    return;
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    buffer.pop_back();
    return;
  }
  else if (IsKeyPressed(KEY_ENTER)) {
    parseBuffer();
    toggleCommandMode();
    return;
  }

  char unicode = GetCharPressed();
  if (unicode != 0) {
    buffer.push_back(unicode);
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

  buffer.clear();
  PlayerActor::setControllable(!command_mode);
}

void CommandSystem::parseBuffer() {
  PLOGD << "Command sent: '" << buffer << "'";
  string::iterator iterator = buffer.begin();
  string first_word = findNextWord(buffer, iterator, true);

  auto result = command_table.find(first_word);
  if (result == command_table.end()) {
    PLOGE << "Invalid Command!";
    return;
  }

  CommandType type = result->second;
  interpretCommand(type, iterator);
}

void CommandSystem::interpretCommand(CommandType type, 
                                     string::iterator &iterator) 
{
  PLOGD << "Interpreting command type.";
  switch (type) {
    case CHANGE_MAP: {
      string map_name = findNextWord(buffer, iterator);
      string spawn_name = findNextWord(buffer, iterator);

      loadMapCommand(map_name, spawn_name);
      break;
    }
  }
}

void CommandSystem::drawBuffer() {
  if (!command_mode) {
    return;
  }

  float txt_size = Game::sm_font.baseSize;
  DrawRectangleV({0, 220}, {426, txt_size}, {0, 0, 0, 128});
  DrawTextEx(Game::sm_font, buffer.c_str(), {32, 220}, txt_size, -3, RED);
}


string findNextWord(string &buffer, string::iterator &iterator,
                    bool uppercase) 
{
  PLOGD << "Searching for next word in buffer: '" << buffer << "'";
  if (iterator == buffer.end()) {
    PLOGD << "Iterator has reached end of buffer.";
    return " ";
  }

  while (*iterator == ' ' && iterator != buffer.end()) {
    iterator++;
  }

  string word;
  while (true) {
    if (iterator == buffer.end() || *iterator == ' ') {
      break;
    }

    if (uppercase) {
      *iterator = std::toupper(*iterator);
    }
    word.push_back(*iterator);
    iterator++;
  }

  PLOGE << "Word returned: '" << word << "'";
  return word;
}

void loadMapCommand(string map_name, string spawn_name) {
  if (map_name.empty()) {
    PLOGE << "Expecting 1 or more arguments, but found none!";
    return;
  }

  PLOGD << "Now executing command.";
  FieldEventHandler::raise<LoadMapEvent>(LOAD_MAP, map_name, spawn_name);
}
