#pragma once
#include <memory>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "menu/hud/portrait.h"
#include "menu/hud/rating.h"
#include "menu/panels/dialog.h"
#include "system/sprite_atlas.h"


class SetupConfirm : public Panel {
public:
  SetupConfirm(GameMode mode, SubWeaponID weapon, CompanionID companion);
  ~SetupConfirm();

  void setupInfo(GameMode mode, SubWeaponID weapon, 
                 CompanionID companion);
  void setupRating(SubWeaponID weapon, CompanionID companion);
  void setupDialog();

  void update() override;
  void transitionLerp();
  void draw() override;

  PromptOptions response;
private:
  SpriteAtlas *atlas;
  Rectangle title_sprite;
  Vector2 title_position = {16, 16};

  Texture info_frame;
  Rectangle info_source = {0, 0, 163, 0};

  Vector2 start1 = {0, -1};
  Vector2 end1 = {426, -1};

  Vector2 start2 = {0, 241};
  Vector2 end2 = {426, 241};

  Portrait mary_port = Portrait({65, 48});
  Portrait comp_port = Portrait({131, 48});

  Font *med_font;
  int med_size;

  std::string m_text;
  Color m_color = WHITE;
  Vector2 m_position;

  std::string w_text;
  Vector2 w_position;

  std::string c_text;
  Vector2 c_position;

  std::unique_ptr<StarRating> stars;
  std::unique_ptr<DialogPanel> panel;
};
