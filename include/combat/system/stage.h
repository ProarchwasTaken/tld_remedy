#pragma once
#include <raylib.h>
#include <string>


class CombatStage {
public:
  void loadStage(std::string name);
  void drawBackground();
  void drawOverlay();
private:
  Texture background;
  Texture overlay;
};
