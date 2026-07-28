#include <cassert>
#include <memory>
#include <cstddef>
#include <vector>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/session.h"
#include "utils/input.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "system/sprite_atlas.h"
#include "menu/hud/savefile.h"
#include "menu/panels/dialog.h"
#include "menu/panels/file_select.h"
#include <plog/Log.h>

using std::make_unique, std::unique_ptr, std::vector, std::string;


FileSelectPanel::FileSelectPanel(Session *session) {
  id = PanelID::FILE_SELECT;
  this->session = session;

  setupMainText();
  setupSaveFiles();
  updateSelected();
  
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

  panel.reset();

  if (load_game) {
    Game::loadGame(*selected);
  }
}

void FileSelectPanel::setupMainText() {
  PLOGI << "Setting up main panel text.";

  if (session != NULL) {
    text = "--- SAVE GAME ---";
    save_mode = true;
  }
  else {
    text = "--- LOAD GAME ---";
    save_mode = false;
  }

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  text_position = TextUtils::alignCenter(text.c_str(), {213, 24}, *font, 
                                         -2, 0);
  Vector2 scale = MeasureTextEx(*font, text.c_str(), txt_size, -2);
  text_rect = {text_position.x , text_position.y, scale.x, scale.y};
}

void FileSelectPanel::setupSaveFiles() {
  PLOGI << "Initializing save files.";

  Vector2 position = {112, 51};
  for (int x = 0; x < 3; x++) {
    SessionID file_id = options[x];
    unique_ptr<SaveFile> &file = save_files[x];

    assert(file == nullptr);
    file = make_unique<SaveFile>(position, file_id);  

    if (!save_mode && !file->valid) {
      disallowed.emplace(file_id);
    }

    position.y += 48;
  }
}

void FileSelectPanel::updateSelected() {
  selected = NULL;
  for (int x = 0; x < 3; x++) {
    SessionID *file_id = &options[x];

    if (disallowed.find(*file_id) == disallowed.end()) {
      selected = file_id;
      PLOGI << "Placing reticle on File " << *selected;
      break;
    }
  }

  assert(selected != NULL);
}

void FileSelectPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();

    for (auto &file : save_files) {
      file->update(state, clock);
    }
    return;
  }

  if (panel == nullptr) {
    blink_clock += Game::deltaTime();
    menuNavigation();
  }
  else {
    panelLogic();
  }
}

void FileSelectPanel::panelLogic() {
  assert(panel != nullptr);
  panel->update();

  if (!panel->terminate) {
    return;
  }

  assert(panel->selected != NULL);
  promptHandling(*panel->selected);

  panel.reset();
}

void FileSelectPanel::promptHandling(PromptOptions response) {
  if (response == PromptOptions::NO) {
    return;
  }

  if (save_mode) {
    assert(session != NULL);
    
    auto &file = save_files[*selected - 1];
    file->updateText(session, true);
    file->valid = true;

    PLOGI << "Saving to external file " << *selected;
    Game::saveGame(session, *selected);
  }
  else {
    PLOGI << "Preparing to load file " << *selected;
    load_game = true;
    state = PanelState::CLOSING;
  }
}

void FileSelectPanel::menuNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, &disallowed);
    sfx->play("menu_navigate");
    blink_clock = 0;
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    sfx->play("menu_navigate");
    blink_clock = 0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    openDialog();
    sfx->play("menu_select");
    blink_clock = 0;
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void FileSelectPanel::openDialog() {
  vector<string> dialog;
  if (save_mode) {
    string text = TextFormat("Save to file %i?\n"
                             "(Existing data will be overwritten.)",
                             *selected);
    dialog.push_back(text);
  }
  else {
    string text = TextFormat("Load file %i?", *selected);
    dialog.push_back(text);
  }

  Vector2 position = {97, 183};
  panel = make_unique<DialogPanel>(position, dialog, true);
}

void FileSelectPanel::draw() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  DrawRectangleRec(text_rect, BLACK);
  DrawTextEx(*font, text.c_str(), text_position, txt_size, -2, WHITE);

  for (auto &file : save_files) {
    file->draw();

    if (state == PanelState::READY && *selected == file->file_id) {
      Vector2 position = Vector2Subtract(file->main_position, {4, 4});
      reticle.draw(position, blink_clock / 2);
    }
  }

  if (panel != nullptr) {
    panel->draw();
  }
}
