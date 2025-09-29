#pragma once
#include <array>
#include <string>
#include <raylib.h>
#include "base/scene.h"
#include "data/session.h"
#include "data/keybinds.h"
#include "menu/hud/party.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


enum class CampMenuOption {
  ITEMS,
  TECHS,
  STATUS,
  CONFIG,
  END_GAME,
};


class CampMenuScene : public Scene {
public:
  CampMenuScene(Session *session);
  ~CampMenuScene();

  void update() override;
  void optionNavigation();
  void selectOption();
  void panelTermination();

  void offsetBars();
  void optionTimer();

  void draw() override;
  void drawTopBar();
  void drawHeader(Vector2 position);
  void drawTopInfo(Vector2 position);

  void drawBottomBar();
  void drawBottomInfo(Vector2 position);
  void drawSupplyCount(Font *font, int txt_size, Vector2 position);
  void drawPlaytime(Font *font, int txt_size, Vector2 position);

  void drawDescription(Font *font, int txt_size, Vector2 position);
  std::string getDescription(CampMenuOption option);

  void drawOptions();
  void baseOptionLerp(Vector2 &base_position, Color &base_tint);
  void selectedOptionLerp(Vector2 &position);
  void unselectedOptionLerp(float &x, unsigned char &alpha);
  std::string getOptionName(CampMenuOption option);

  static SpriteAtlas atlas;
  static SpriteAtlas menu_atlas;
private:
  Session *session;
  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  enum {
    OPENING, 
    READY, 
    CLOSING, 
    OPENING_PANEL, 
    CLOSING_PANEL
  } state = OPENING;

  float state_clock = 0.0;
  float state_time = 0.8;

  float panel_clock = 0.0;
  float panel_time = 0.4;

  std::array<CampMenuOption, 5> options = {
    CampMenuOption::ITEMS,
    CampMenuOption::TECHS,
    CampMenuOption::STATUS,
    CampMenuOption::CONFIG,
    CampMenuOption::END_GAME
  };

  std::array<CampMenuOption, 5>::iterator selected = options.begin();
  float opt_switch_clock = 0.0;
  float opt_switch_time = 0.1;

  Texture main_bar;
  float bar_offset = 0;
  float offset_speed = 105;

  PartyHud plr_hud = PartyHud({217, 152});
  PartyHud com_hud = PartyHud({321, 152});

  std::string description;
  Color desc_color = WHITE;

  static constexpr Vector2 top_position = {0, 0};
  static constexpr Vector2 bottom_position = {0, 200};
  static constexpr Vector2 option_position = {9, 42};
};
