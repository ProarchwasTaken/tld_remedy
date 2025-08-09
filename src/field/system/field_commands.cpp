#include <raylib.h>
#include <string>
#include <cctype>
#include <cassert>
#include "enums.h"
#include "data/field_event.h"
#include "game.h"
#include "scenes/field.h"
#include "field/system/field_handler.h"
#include "field/system/field_commands.h"
#include "field/actors/player.h"
#include <plog/Log.h>

using std::string;
string findNextWord(string &buffer, string::iterator &iterator, 
                    bool uppercase = false);
void loadMapCommand(string map_name, string spawn_name);
void deleteEntityCommand(string argument);
void saveCommand();
void initCombatCommand();
void setSuppliesCommand(string argument);
void setLifeCommand(string target, string value);


CommandSystem::CommandSystem() {
  assert(Game::devmode);
  PLOGD << "Initialized debug command system.";
}

CommandSystem::~CommandSystem() {
  PLOGD << "Reseting debug command system.";
}

void CommandSystem::assignScene(FieldScene *scene) {
  PLOGI << "Assigned scene to command system.";
  CommandSystem::scene = scene;
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
    case CommandType::CHANGE_MAP: {
      string map_name = findNextWord(buffer, iterator);
      string spawn_name = findNextWord(buffer, iterator);

      loadMapCommand(map_name, spawn_name);
      break;
    }
    case CommandType::SAVE: {
      saveCommand();
      break;
    }
    case CommandType::INIT_COMBAT: {
      initCombatCommand();
      break;
    }
    case CommandType::DELETE_ENT: {
      string argument = findNextWord(buffer, iterator);

      deleteEntityCommand(argument);
      break;
    }
    case CommandType::SET_SUPPLIES: {
      string argument = findNextWord(buffer, iterator);

      setSuppliesCommand(argument);
      break;
    }
    case CommandType::SET_LIFE: {
      string target = findNextWord(buffer, iterator, true);
      string value = findNextWord(buffer, iterator);

      setLifeCommand(target, value);
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

  PLOGD << "Word returned: '" << word << "'";
  return word;
}

void loadMapCommand(string map_name, string spawn_name) {
  if (map_name.empty()) {
    PLOGE << "Expecting 1 or more arguments, but found none!";
    return;
  }

  PLOGD << "Now executing command.";
  FieldHandler::raise<LoadMapEvent>(FieldEVT::LOAD_MAP, map_name, 
                                    spawn_name);
}

void saveCommand() {
  PLOGD << "Now executing command.";
  FieldHandler::raise<FieldEvent>(FieldEVT::SAVE_SESSION);
}

void initCombatCommand() {
  PLOGD << "Now executing command.";
  FieldHandler::raise<FieldEvent>(FieldEVT::INIT_COMBAT);
}

void deleteEntityCommand(string argument) {
  if (argument.empty()) {
    PLOGE << "Expecting 1 or more arguments, but found none!";
    return;
  }

  for (char letter : argument) {
    if (!std::isdigit(letter)) {
      PLOGE << "Invalid Argument! Expecting whole number!";
      return;
    }
  }

  PLOGD << "Now executing command.";
  int entity_id = std::stoi(argument);

  FieldHandler::raise<DeleteEntityEvent>(FieldEVT::DELETE_ENTITY, 
                                         entity_id);
}

void setSuppliesCommand(string argument) {
  if (argument.empty()) {
    PLOGE << "Expecting 1 or more arguments, but found none!";
    return;
  }

  for (char letter : argument) {
    if (!std::isdigit(letter)) {
      PLOGE << "Invalid Argument! Expecting whole number!";
      return;
    }
  }

  PLOGD << "Now executing command.";
  int value = std::stoi(argument);

  FieldHandler::raise<SetSuppliesEvent>(FieldEVT::CHANGE_SUPPLIES, value);
}

void setLifeCommand(string target, string value) {
  if (target.empty()) {
    PLOGE << "Target is not specified!";
    return;
  }

  if (value.empty()) {
    PLOGE << "Expecting a value to set life to!";
    return;
  }

  bool detected_decimal = false;
  for (char letter : value) {
    if (std::isdigit(letter)) {
      continue;
    }

    if (!detected_decimal && letter == '.') {
      detected_decimal = true;
    }
    else {
      PLOGE << "'" << value << "' is not a floating point number!";
      return;
    }
  }

  PLOGD << "Now executing command.";
  float life_value = std::stof(value);
  
  if (target == "PLAYER") {
    FieldHandler::raise<SetPlrLifeEvent>(FieldEVT::CHANGE_PLR_LIFE, 
                                         life_value);
  }
  else {
    PLOGD << "'" << target << "' is not a valid target.";
  }
}

