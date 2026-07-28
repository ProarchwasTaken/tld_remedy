#include <cassert>
#include <cmath>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/session.h"
#include "utils/text.h"
#include "system/sprite_atlas.h"
#include "menu/hud/savefile.h"
#include <plog/Log.h>

SpriteAtlas SaveFile::atlas("menu", "savefile");


SaveFile::SaveFile(Vector2 position, SessionID file_id) {
  assert(file_id != UNSAVED && file_id != TEMPORARY);
  main_position = position;
  this->file_id = file_id;
  atlas.use();

  try {
    Session data = Game::validateSession(file_id);
    updateText(&data, false);
    valid = true;
    PLOGI << "File " << file_id << "has been initialized.";
  } 
  catch (SessionException error_code) {
    PLOGI << "Marking Save File " << file_id << " as empty.";
    frame = atlas.sprites[0];
    frame_color = WHITE;
    valid = false;
  }

  frame.height = 0;
}

SaveFile::~SaveFile() {
  atlas.release();
}

void SaveFile::updateText(Session *data, bool active_playtime) {
  assert(data != NULL);
  frame = atlas.sprites[1];
  location = data->location;

  if (data->game_mode == GameMode::INDIFFERENCE) {
    gamemode = "Indifference";
    frame_color = Game::palette[41];
    pattern_color = Game::palette[40];
    gm_color = frame_color;
  }
  else {
    gamemode = "Reverence!";
    frame_color = Game::palette[33];
    pattern_color = Game::palette[32];
    gm_color = pattern_color;
  }

  switch (data->companion.companion_id) {
    case CompanionID::ERWIN: {
      party = "Mary & Erwin";
      break;
    }
    case CompanionID::XANDER: {
      party = "Mary & Xander";
      break;
    }
  }

  long seconds;
  if (active_playtime) {
    seconds = std::floor(Game::playtime());;
  }
  else {
    seconds = std::floor(data->playtime);
  }

  int minutes = (seconds / 60) % 60;
  int hours = seconds / 3600;
  playtime = TextFormat("%02i:%02i", hours, minutes);
}

void SaveFile::update(PanelState state, float clock) {
  switch (state) {
    case PanelState::READY: {
      frame.height = frame_height;
      return;
    }
    case PanelState::OPENING: {
      frame.height = Lerp(0, frame_height, clock);
      break;
    }
    case PanelState::CLOSING: {
      frame.height = Lerp(frame_height, 0, clock);
      break;
    }
  }
}

void SaveFile::draw() {
  DrawTextureRec(atlas.sheet, frame, main_position, frame_color);

  if (valid && frame.height == frame_height) {
    DrawTextureRec(atlas.sheet, atlas.sprites[2], main_position, 
                   pattern_color);
    drawSaveInfo();
  }
}

void SaveFile::drawSaveInfo() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  Vector2 position = Vector2Add(main_position, {2, 1});

  DrawTextEx(*font, location.c_str(), position, txt_size, -2, 
             Game::palette[22]);

  position.y += 13;
  DrawTextEx(*font, "Game Mode", position, txt_size, -2, WHITE);

  position.y += 13;
  DrawTextEx(*font, "Party", position, txt_size, -2, WHITE);

  position = Vector2Add(main_position, {75, 14});
  DrawTextEx(*font, gamemode.c_str(), position, txt_size, -2, gm_color);

  position = Vector2Add(main_position, {64, 27});
  DrawTextEx(*font, party.c_str(), position, txt_size, -2, WHITE);

  position = Vector2Add(main_position, {199, 1});
  position = TextUtils::alignRight(playtime.c_str(), position, *font, -2, 
                                   0);
  DrawTextEx(*font, playtime.c_str(), position, txt_size, -2, WHITE);
}
