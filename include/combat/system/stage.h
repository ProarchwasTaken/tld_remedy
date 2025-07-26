#pragma once
#include <raylib.h>
#include <string>


class CombatStage {
public:
  void loadStage(std::string name);
  static void tintStage(Color tint);

  void update();

  void drawBackground();
  void drawOverlay();
private:
  Texture background;
  Texture overlay;

  static Color tint;
  static bool tint_changed;
};
