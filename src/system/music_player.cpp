#include <cassert>
#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <raylib.h>
#include "game.h"
#include "system/music_player.h"
#include <plog/Log.h>

using std::string, nlohmann::json, nlohmann::basic_json, std::ifstream;


MusicPlayer::~MusicPlayer() {
  UnloadMusicStream(primary);
}

void MusicPlayer::prepare(string song_name) {
  UnloadMusicStream(primary);

  PLOGI << "Retrieving music data.";
  ifstream file("data/bgm.json");
  if (!file.is_open()) {
    PLOGE << "File not found!";
    file.close();
    throw;
  }

  loop_start = -1;
  loop_end = -1;

  json data = json::parse(file);
  PLOGI << "Loading song: " << song_name;

  for (basic_json bgm_data : data["bgm"]) {
    string name = bgm_data["name"];
    string filename = bgm_data["filename"];

    if (song_name != name) {
      continue;
    }

    string path = "audio/bgm/" + filename;
    primary = LoadMusicStream(path.c_str());
    PauseMusicStream(primary);

    if (bgm_data.find("loop_start") != bgm_data.end()) {
      loop_start = bgm_data["loop_start"];
      PLOGD << "Loop Start: " << loop_start;
    }

    if (bgm_data.find("loop_end") != bgm_data.end()) {
      loop_end = bgm_data["loop_end"];
      PLOGD << "Loop End: " << loop_end;
    }

    looping = loop_start <= 0 && loop_end < 0;
    break;
  }
}

void MusicPlayer::play() {
  assert(IsMusicReady(primary));
  float master_volume = Game::settings.master_volume;
  float bgm_volume = Game::settings.bgm_volume;
  SetMusicVolume(primary, (volume * bgm_volume) * master_volume);
  ResumeMusicStream(primary);
}

void MusicPlayer::update() {
  if (!looping || !IsMusicStreamPlaying(primary)) {
    return;
  }

  float position = GetMusicTimePlayed(primary);
  if (position >= loop_end) {
    SeekMusicStream(primary, loop_start);
  }
}
