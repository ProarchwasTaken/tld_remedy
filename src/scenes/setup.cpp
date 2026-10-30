#include <cassert>
#include <memory>
#include "enums.h"
#include "game.h"
#include "system/sprite_atlas.h"
#include "menu/panels/weapon.h"
#include "menu/panels/companion.h"
#include "menu/panels/setup_confirm.h"
#include "scenes/setup.h"
#include <plog/Log.h>

using std::make_unique;
SpriteAtlas SetupScene::atlas("menu", "setup_menu");


SetupScene::SetupScene() {
  scene_id = SceneID::SETUP;
  atlas.use();

  initPanel();
  PLOGI << "SetupScene has been initialized.";
}

SetupScene::~SetupScene() {
  if (panel != nullptr) {
    panel.reset();
  }

  atlas.release();
}

void SetupScene::initPanel() {
  switch (phase) {
    case SetupPhase::SELECT_GAMEMODE: {
      // TODO
      break;
    }
    case SetupPhase::SELECT_SUBWEAPON: {
      panel = make_unique<WeaponPanel>();
      break;
    }
    case SetupPhase::SELECT_COMPANION: {
      panel = make_unique<CompanionPanel>();
      break;
    }
    case SetupPhase::CONFIRMATION: {
      panel = make_unique<SetupConfirm>(gamemode, sub_weapon, companion);
      break;
    }
  }

  assert(panel != nullptr);
  panel_mode = true;
}

void SetupScene::update() {
  if (panel == nullptr) {
    initPanel();
  }

  panel->update();

  if (panel->terminate) {
    panelTermination();
  }
}

void SetupScene::panelTermination() {
  switch (panel->id) {
    case PanelID::WEAPON: {
      WeaponPanel *w_panel = static_cast<WeaponPanel*>(panel.get());
      if (w_panel->exiting_out) {
        Game::loadTitleScreen();
      }
      else {
        sub_weapon = static_cast<SubWeaponID>(*w_panel->selected);
        phase = SetupPhase::SELECT_COMPANION;
      }

      break;
    }
    case PanelID::COMPANION: {
      CompanionPanel *c_panel = static_cast<CompanionPanel*>(panel.get());
      if (c_panel->exiting_out) {
        phase = SetupPhase::SELECT_SUBWEAPON;
      }
      else {
        companion = static_cast<CompanionID>(*c_panel->selected);
        phase = SetupPhase::CONFIRMATION;
      }

      break;
    }
    case PanelID::SETUP_CONFIRM: {
      SetupConfirm *sc_panel = static_cast<SetupConfirm*>(panel.get());
      if (sc_panel->response == PromptOptions::YES) {
        Game::newGame(gamemode, sub_weapon, companion);
        Game::menu_sfx.play("menu_select");
      }
      else {
        phase = SetupPhase::SELECT_SUBWEAPON;
        Game::menu_sfx.play("menu_cancel");
      }

      break;
    }
    default: {
      break;
    }
  }

  panel.reset();
}

void SetupScene::draw() {
  if (panel_mode && panel != nullptr) {
    panel->draw();
  }
}
