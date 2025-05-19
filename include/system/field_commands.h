#pragma once
#include <string>
#include "scenes/field.h"


class CommandSystem {
public:
  CommandSystem(FieldScene *scene);
  ~CommandSystem();

  static void process();
  static void toggleCommandMode();
private:
  static FieldScene *scene;

  inline static std::string buffer;
  inline static bool command_mode = false;
};
