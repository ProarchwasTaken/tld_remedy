#include <string>
#include <array>
#include <unordered_set>
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/panels/dialog.h"
#include "menu/hud/portrait.h"

enum class DiagnoseOptions { 
  LIFE,
  EFFECT_1,
  EFFECT_2,
  EFFECT_3
};


class DiagnosePanel : public Panel {
public:
  DiagnosePanel(Session *session, SpriteAtlas *rest_atlas);
  ~DiagnosePanel();

  void updateDisallowed();

  void update() override;
  void heightLerp();
  void panelTermination();
  void promptHanding();
  void menuNavigation();
  void selectOption();

  void enterHealMode();
  void openEffectDialog();

  void healModeInput();
  void incHealSegments(float life, float max_life);
  void decHealSegments(float life, float max_life);
  void openHealDialog();
  void applyHeal();
  float calculateToBeHealed(float life, float max_life);

  void cureEffect();

  void draw() override;
  void drawSupplyCount();
  void drawMemberName();
  void drawPortrait();

  void drawCursor();
  void drawLife();
  void drawLifeText(float life, float max_life, Color color);
  void drawGauge(float life, float max_life, Color default_color);

  void drawStatus();
  void setStatusColors(StatusID id, Color &name_color, Color &desc_color, 
                       Color &cost_color);
  std::string getStatusName(StatusID id);
  std::string getStatusDesc(StatusID id);
  int getSupplyCost(StatusID id);
private:
  SpriteAtlas *menu_atlas;
  SpriteAtlas *rest_atlas;

  MenuKeybinds *keybind;
  SoundAtlas *sfx;

  float percentage = 0.0;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {76, 24};

  std::array<Character*, 2> party;
  std::array<Character*, 2>::iterator current_member;
  int *supplies;

  std::array<DiagnoseOptions, 4> options = {
    DiagnoseOptions::LIFE,
    DiagnoseOptions::EFFECT_1,
    DiagnoseOptions::EFFECT_2,
    DiagnoseOptions::EFFECT_3
  };
  std::array<int, 4> y_values = {46, 68, 118, 168};
  std::array<DiagnoseOptions, 2>::iterator selected = options.begin();
  std::unordered_set<DiagnoseOptions> disallowed;
  float blink_clock = 0.0;

  bool heal_mode = false;
  int heal_segments = 0;
  float to_be_healed = 0;

  Portrait portrait = Portrait({81, 54});

  bool panel_mode = false;
  std::unique_ptr<DialogPanel> panel;
};
