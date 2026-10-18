#pragma once
#include "base/panel.h"
#include "system/sprite_atlas.h"


class WeaponPanel : public Panel {
public:
  WeaponPanel();
  ~WeaponPanel();

  void update() override;
  void draw() override;
private:
  SpriteAtlas *atlas;
};
