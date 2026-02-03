#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "menu/panels/dialog.h"
#include "scenes/game_over.h"

using std::vector, std::string, std::make_unique;


GameOverScene::GameOverScene() {
  scene_id = SceneID::GAME_OVER;
}

void GameOverScene::startDialog() {
  vector<string> dialog = {
    "Martyrs aren't expected to live;\n"
    "let alone succeed.",

    "So many ways it could go wrong, most\n"
    "of which conjured by their own\n"
    "imagination and paranoia.",

    "Mary had always been aware of how\n"
    "insignificant he was in the grand\n"
    "scheme of things.",

    "After all, He was a human who came\n"
    "from nothing, and would inevitably\n"
    "return to it in death.",

    "He was never significant enough to\n"
    "play a major role...\n"
    "And yet, he lasted longer than most.",

    "It was because of that fact that I\n"
    "decided to deem him as a Martyr\n"
    "in the first place.",

    "For they are the stepping stones that\n"
    "set the initial conditions for the\n"
    "real game to begin.",

    "While a hero is one who defeats the\n"
    "villain, a Martyr is one who proves\n"
    "that they can bleed.",
 
    "If Mary fails, then someone else\n"
    "will surely take his place.\n"
    "That's just how expendable he is.",

    "Of course, you won't settle for just\n"
    "that... Wouldn't you, Mary?"
  };

  panel = make_unique<DialogPanel>((Vector2){16, 8}, dialog, true, 
                                   false, 1.5);
  panel_mode = true;
}

void GameOverScene::startClosingDialog() {
  vector<string> dialog = {
    "...",
    "If that's your decision...",
    "Just remember, Mary, time will always\n"
    "keep moving forward; With or without\n"
    "you.",
    "You may rest now, but eventually,\n"
    "you will have to get up."
  };

  panel = make_unique<DialogPanel>(Vector2{16, 8}, dialog, false, false,
                                   1.5);
  panel_mode = true;
}

void GameOverScene::update() {
  if (!ready) {
    startup();
    return;
  }

  if (panel_mode) {
    panelLogic();
  }
  else if (exiting) {
    Game::loadTitleScreen();
  }
}

void GameOverScene::startup() {
  start_clock += Game::deltaTime() / start_time;
  start_clock = Clamp(start_clock, 0.0, 1.0);

  if (start_clock == 1.0) {
    startDialog();
    ready = true;
  }
}

void GameOverScene::panelLogic() {
  assert(panel != nullptr);
  panel->update();

  if (!panel->terminate) {
    return;
  }

  if (exiting) {
    Game::fadeout(1.0);
  }

  bool open_closing_dialog = false;
  if (panel->id == PanelID::DIALOG) {
    open_closing_dialog = responseHandling();
  }

  panel.reset();
  panel_mode = false;

  if (open_closing_dialog) {
    startClosingDialog();
  }
}

bool GameOverScene::responseHandling() {
  assert(panel->id == PanelID::DIALOG);
  DialogPanel *dialog = static_cast<DialogPanel*>(panel.get());

  if (dialog->selected == NULL) {
    return false;
  }

  PromptOptions response = *dialog->selected;

  switch (response) {
    case PromptOptions::NO: {
      exiting = true;
      return true;
    }
    case PromptOptions::YES: {
      bool successful = Game::loadSession();
      if (!successful) {
        exiting = true;
      }
      return false;
    }
  }
}

void GameOverScene::draw() {
  if (panel_mode) {
    panel->draw();
  }
}
