#pragma once
#include <string>
#include <raylib.h>


class MusicPlayer {
public:
  ~MusicPlayer();

  void prepare(std::string song_name);
  void play();

  void update();
private:
  Music primary;

  float volume = 1.0;
  bool looping = false;

  float loop_start = 0;
  float loop_end = 0;
};
