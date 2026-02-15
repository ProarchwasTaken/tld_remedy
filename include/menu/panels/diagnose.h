#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class DiagnosePanel : public Panel {
public:
  DiagnosePanel(Session *session, SpriteAtlas *rest_atlas);
  ~DiagnosePanel();

  void update() override;
  void heightLerp();

  void draw() override;
private:
  SpriteAtlas *menu_atlas;
  SpriteAtlas *rest_atlas;

  MenuKeybinds *keybinds;
  SoundAtlas *sfx;

  float percentage = 0.0;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {76, 24};
};
