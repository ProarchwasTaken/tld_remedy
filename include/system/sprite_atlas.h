#pragma once
#include <string>
#include <vector>
#include <raylib.h>


class SpriteAtlas {
public:
  SpriteAtlas(std::string directory, std::string sprite_group);
  ~SpriteAtlas();

  void setup();
  void reset();

  void use();
  void release();

  Rectangle source;
  Texture sheet;
  std::vector<Rectangle> sprites;
private:
  std::string sprite_group;
  std::string directory;
  unsigned int user_count = 0;
};
