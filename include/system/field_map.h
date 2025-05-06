#pragma once
#include <raylib.h>
#include <string>


class FieldMap {
public:
  ~FieldMap();
  void loadMap(std::string map_name);
  void draw();

  bool ready = false;
private:
  Texture base;
};
