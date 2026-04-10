#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <random>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "menu/panels/dialog.h"
#include "scenes/game_over.h"

using std::vector, std::string, std::make_unique, 
std::uniform_int_distribution;


GameOverScene::GameOverScene(string reason) {
  scene_id = SceneID::GAME_OVER;
  this->reason = reason;
}

vector<string> GameOverScene::getDialog() {
  uniform_int_distribution<int> range(0, 1);
  int num = range(Game::RNG);


  switch (num) {
    case 0: {
      return {
        "It all starts with the first victim.",
        "While they may have stood no chance of\n"
        "surviving what was ahead of them.",
        "It's through their death that the\n"
        "threat's existence becomes known.",
        "And then the next person comes in line.",
        "They make it a bit further than the\n"
        "one before them, they fall, and the\n"
        "cycle repeats.",
        "The process continues until the threat\n"
        "is neutralized.",
        "With only the one at the end of the\n"
        "chain being branded a Hero.",
        "That is the path of thorns that must\n"
        "be walked in order for true change to\n"
        "occur.",
        "The eternal oscillation between the\n"
        "Dark Age, and the Golden Age is the\n"
        "mechanism that runs this 'Reality'",
        "Even so, the Chosen One is ultimately\n"
        "just the one that succeeds in bringing\n"
        "about change.",
        "The door is always open. Why not try\n"
        "your luck, and choose to be the one?"
      };
    }
    default: {
      return {
        "Mary had always been aware of how\n"
        "insignificant he was in the grand\n"
        "scheme of things.",
        "After all, he was a human who came\n"
        "from nothing, and would inevitably\n"
        "return to it in death.",
        "If there is someone who could defeat\n"
        "the villain, it certainly isn't him.",
        "And yet, he has lasted longer than\n"
        "most.",
        "Rather than submitting to his\n"
        "circumstances, he chose to rise to\n"
        "the occasion.",
        "Instead of allowing himself to go\n"
        "through the motions, he willingly\n"
        "swam against the current.",
        "The decision to trek the path of\n"
        "thorns was entirely his own.",
        "All for the chance at a better life.\n"
        "If not for him... then maybe someone\n"
        "else.",
        "If there is a title for people like\n"
        "him, it wouldn't be \"Hero\".",
        "Although... It wouldn't hurt to strive\n"
        "for old dream of heroism... right?"};
    }
  }
}

void GameOverScene::startDialog() {
  vector<string> dialog = getDialog();
  panel = make_unique<DialogPanel>((Vector2){16, 8}, dialog, true, 
                                   true, 1.5);
  panel_mode = true;
}

void GameOverScene::startClosingDialog() {
  vector<string> dialog = {
    "...",
    "Just remember, Mary.",
    "Without power, you can change nothing..\n",
    "But what you can do is spark that\n"
    "change.",
    "Even if you won't be there to see it,\n"
    "no action is without consequence.",
    "So you better make it count when the\n"
    "time comes, alright?"
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
  if (start_clock < 0.33) {
    drawGameOverText();
  }

  if (panel_mode) {
    panel->draw();
  }
}

void GameOverScene::drawGameOverText() {
  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize * 1.5;

  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;

  Color red = Game::palette[32];
  Color yellow = Game::palette[22];

  DrawTextEx(*med_font, "GAME OVER", {2, 206}, med_size, 0, red);
  DrawTextEx(*sm_font, reason.c_str(), {2, 224}, sm_size, -3, yellow);
}
