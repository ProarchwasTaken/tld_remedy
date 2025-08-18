#pragma once
#include <array>
#include <string>
#include <memory>
#include <unordered_set>
#include <raylib.h>
#include "base/scene.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"

enum class TitleOption {
  NEW_GAME,
  LOAD_GAME,
  CONFIG
};

class TitleScene : public Scene {
public:
  TitleScene();
  ~TitleScene();

  void update() override;
  void optionNavigation();
  void selectOption();

  void draw() override;

  std::string getOptionText(TitleOption id);
  void drawOptions(Font *font, int txt_size);
  void drawCursor(Vector2 position, Color color);
private:
  MenuKeybinds *keybinds;
  SoundAtlas *sfx;
  SpriteAtlas menu_atlas = SpriteAtlas("menu", "menu_elements");

  std::array<TitleOption, 3> options = {
    TitleOption::NEW_GAME,
    TitleOption::LOAD_GAME,
    TitleOption::CONFIG
  };

  std::array<TitleOption, 3>::iterator selected = options.begin();
  float blink_clock = 0.0;

  bool valid_session;
  std::unordered_set<TitleOption> disallowed;

  bool panel_mode = false;
  std::unique_ptr<Panel> panel;
};
