#pragma once
#include <string>
#include <map>
#include <raylib.h>


class SoundAtlas {
public:
  SoundAtlas(std::string category);
  ~SoundAtlas();

  void setup();
  void reset();

  int users() {return user_count;}

  void use();
  void release();
private:
  std::string category;
  std::map<std::string, Sound> sound_table;

  unsigned int user_count = 0;
};
