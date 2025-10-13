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

  int users() {return user_count;};

  void use();
  void release();

  Texture getTexturefromSprite(int index);

  Rectangle source;
  Texture sheet;
  std::vector<Rectangle> sprites;
private:
  std::string sprite_group;
  std::string directory;
  unsigned int user_count = 0;
};
