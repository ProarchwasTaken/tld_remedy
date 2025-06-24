#include <cassert>
#include <raylib.h>
#include <string>
#include "combat/system/stage.h"
#include <plog/Log.h>

using std::string;


void CombatStage::loadStage(string name) {
  PLOGI << "Loading stage: '" << name << "'";
  string directory = "graphics/stages/" + name;
  assert(DirectoryExists(directory.c_str()));

  string background_path = directory + "/background.png";
  assert(FileExists(background_path.c_str()));

  string overlay_path = directory + "/overlay.png";
  assert(FileExists(overlay_path.c_str()));

  background = LoadTexture(background_path.c_str());
  overlay = LoadTexture(overlay_path.c_str());
}

void CombatStage::drawBackground() {
  DrawTextureV(background, {-512, 0}, WHITE);
}

void CombatStage::drawOverlay() {
  DrawTextureV(overlay, {-512, 0}, WHITE);
}

