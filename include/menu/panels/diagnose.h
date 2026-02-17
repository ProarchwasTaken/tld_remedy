#include <array>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/hud/portrait.h"


class DiagnosePanel : public Panel {
public:
  DiagnosePanel(Session *session, SpriteAtlas *rest_atlas);
  ~DiagnosePanel();

  void update() override;
  void heightLerp();
  void menuNavigation();

  void draw() override;
  void drawMemberName();
  void drawPortrait();
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
  Portrait portrait = Portrait({81, 54});
};
