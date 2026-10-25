#include <cassert>
#include <cstring>
#include <memory>
#include "enums.h"
#include "game.h"
#include "system/sprite_atlas.h"
#include "menu/panels/weapon.h"
#include "menu/panels/companion.h"
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
      break;
    }
  }

  assert(panel != nullptr);
  panel_mode = true;
}

void SetupScene::initPlayer(SubWeaponID weapon_id) {
  PLOGI << "Initializing Player data...";
  std::strcpy(player.name, "Mary");
  player.member_id = PartyMemberID::MARY;
  player.weapon_id = weapon_id;

  player.life = 7;
  player.max_life = 15;

  player.init_morale = 10;
  player.max_morale = 15;

  player.offense = 4;
  player.defense = 4;
  player.intimid = 4;
  player.persist = 4;
  player.dexterity = 4;
  player.discipline = 4;

  player.recovery = 1.0;
  player.resilience = 0.5;

  switch (weapon_id) {
    case SubWeaponID::KNIFE: {
      PLOGI << "Applying Knife status changes.";
      player.dexterity += 2;
      player.intimid += 2;
      break;
    }
    case SubWeaponID::BAT: {
      PLOGI << "Applying Bat status changes.";
      player.init_morale = 5;
      player.max_morale -= 5;

      player.offense += 5;
      player.intimid += 2;
      player.discipline -= 2;

      player.recovery -= 0.15;
      break;
    }
    default: {
      PLOGE << "Invalid SubWeapon ID!";
      throw;
    }
  }
}

void SetupScene::initCompanion(CompanionID companion_id) {
  PLOGI << "Initializing Companion data...";

  switch (companion_id) {
    case CompanionID::ERWIN: {
      std::strcpy(companion.name, "Erwin");
      companion.member_id = PartyMemberID::ERWIN;
      companion.companion_id = companion_id;

      companion.life = 20;
      companion.max_life = 20;

      companion.init_morale = 10;
      companion.max_morale = 15;

      companion.offense = 4;
      companion.defense = 4;
      companion.intimid = 6;
      companion.persist = 6;
      companion.dexterity = 5;
      companion.discipline = 5;

      companion.recovery = 1.0;
      companion.resilience = 0.5;
      break;
    }
    case CompanionID::XANDER: {
      std::strcpy(companion.name, "Xander");
      companion.member_id = PartyMemberID::XANDER;
      companion.companion_id = companion_id;

      companion.life = 30;
      companion.max_life = 30;

      companion.init_morale = 0;
      companion.max_morale = 0;

      companion.offense = 12;
      companion.defense = 8;
      companion.intimid = 4;
      companion.persist = 3;
      companion.dexterity = 4;
      companion.discipline = 1;

      companion.recovery = 0.35;
      companion.resilience = 0.75;
      break;
    }
    default: {
      PLOGE << "Invalid Companion ID!";
      throw;
    }
  }

  PLOGI << "Initialized data for: " << companion.name;
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
        SubWeaponID id = static_cast<SubWeaponID>(*w_panel->selected);
        initPlayer(id);
        phase = SetupPhase::SELECT_COMPANION;
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
