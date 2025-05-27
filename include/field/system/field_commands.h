#pragma once
#include <string>
#include <map>
#include "enums.h"
#include "scenes/field.h"


/* The command system allows for the possibility of manually triggering
 * Field events, like changes maps or starting battles. Purely meant for
 * debugging purposes only.*/
class CommandSystem {
public:
  CommandSystem();
  ~CommandSystem();
  static void assignScene(FieldScene *scene);

  static void process();

  static void toggleCommandMode();
  static void parseBuffer();
  static void interpretCommand(CommandType type, 
                               std::string::iterator &iterator);

  static void drawBuffer();
private:
  inline static FieldScene *scene;
  inline static std::map<std::string, CommandType> command_table {
    {"MAP", CHANGE_MAP},
    {"SAVE", SAVE},
    {"LOAD", LOAD},
    {"DELETE_ENTITY", DELETE_ENT},
    {"SET_SUPPLIES", SET_SUPPLIES},
    {"SET_LIFE", SET_LIFE},
  };

  inline static std::string buffer;
  inline static bool command_mode = false;
};
