#pragma once
#include <string>
#include <raylib.h>

enum class StreamState {
  NEUTRAL,
  FADING
};


class MusicPlayer {
public:
  ~MusicPlayer();

  void prepare(std::string song_name);
  void play();
  void stop();
  void pause();
  void resume();

  void setBaseVolume(float volume);
  void updateVolume();
  void fade(float target_volume, float fade_time);

  void update();
  void loopingLogic();
  void volumeFading();
private:
  Music primary;
  StreamState state = StreamState::NEUTRAL;
  bool paused = false;

  float volume = 1.0;
  float start_volume = volume;
  float end_volume = volume;

  float fade_clock = 0.0;
  float fade_time = 1.0;

  bool looping = false;
  float loop_start = 0;
  float loop_end = 0;
};
