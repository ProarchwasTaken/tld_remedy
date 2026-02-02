#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include "enums.h"
#include "menu/panels/dialog.h"
#include "scenes/game_over.h"

using std::vector, std::string, std::make_unique;


GameOverScene::GameOverScene() {
  scene_id = SceneID::GAME_OVER;
  startDialog();
}

void GameOverScene::startDialog() {
  vector<string> dialog = {
    "Martyrs aren't expected to live,\n"
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
    "that... Wouldnt you, Mary?"
  };

  panel = make_unique<DialogPanel>((Vector2){16, 8}, dialog, true, 
                                   false, 1.5);
  panel_mode = true;
}

void GameOverScene::update() {
  if (panel_mode) {
    panelLogic();
  }
}

void GameOverScene::panelLogic() {
  assert(panel != nullptr);
  panel->update();

  if (panel->terminate) {
    panel.reset();
    panel_mode = false;
  }
}

void GameOverScene::draw() {
  if (panel_mode) {
    panel->draw();
  }
}
