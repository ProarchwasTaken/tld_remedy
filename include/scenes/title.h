#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "data/keybinds.h"

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
  void nextOption();
  void prevOption();
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
  bool valid_session;
};
