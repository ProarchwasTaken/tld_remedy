#pragma once
#include <array>
#include "enums.h"
#include "base/panel.h"
#include "system/sprite_atlas.h"


class CompanionPanel : public Panel {
public:
  CompanionPanel();
  ~CompanionPanel();

  void update() override;
  void draw() override;

  std::array<CompanionID, 2> selected;
private:
  SpriteAtlas *atlas;

  std::array<CompanionID, 2> options = {
    CompanionID::ERWIN,
    CompanionID::XANDER
  };
};
