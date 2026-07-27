#include <cassert>
#include <memory>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/session.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "system/sprite_atlas.h"
#include "menu/hud/savefile.h"
#include "menu/panels/file_select.h"
#include <plog/Log.h>

using std::make_unique, std::unique_ptr;


FileSelectPanel::FileSelectPanel(Session *session) {
  id = PanelID::FILE_SELECT;
  selected = options.begin();

  this->session = session;
  if (session != NULL) {
    text = "-- SAVE GAME --";
    save_mode = true;
  }
  else {
    text = "-- LOAD GAME --";
    save_mode = false;
  }

  Font *font = &Game::med_font;
  text_position = TextUtils::alignCenter(text.c_str(), {213, 24}, *font, 
                                         -2, 0);

  setupSaveFiles();
  
  keybinds = &Game::settings.menu_keybinds;
  atlas = &Game::menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  PLOGI << "File Select Panel has been initialized.";
}

FileSelectPanel::~FileSelectPanel() {
  atlas->release();
  sfx->release();

  for (auto &file : save_files) {
    file.reset();
  }
}

void FileSelectPanel::setupSaveFiles() {
  PLOGI << "Initializing save files.";

  Vector2 position = {112, 51};
  for (int x = 0; x < 3; x++) {
    SessionID file_id = options[x];
    unique_ptr<SaveFile> &file = save_files[x];

    assert(file == nullptr);
    file = make_unique<SaveFile>(position, file_id);  
    position.y += 48;
  }
}

void FileSelectPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    return;
  }
}

void FileSelectPanel::draw() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  DrawTextEx(*font, text.c_str(), text_position, txt_size, -2, WHITE);

  for (auto &file : save_files) {
    file->draw();
  }
}
