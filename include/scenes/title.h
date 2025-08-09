#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/scene.h"

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
private:
  std::array<TitleOption, 3> options = {
    TitleOption::NEW_GAME,
    TitleOption::LOAD_GAME,
    TitleOption::CONFIG
  };

  std::array<TitleOption, 3>::iterator selected = options.begin();
  bool valid_session;
};
