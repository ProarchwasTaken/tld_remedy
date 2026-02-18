#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class CraftingPanel : public Panel {
public:
  CraftingPanel(Session *session, SpriteAtlas *rest_atlas);
  ~CraftingPanel();

  void update() override;
  void heightLerp();
  void menuNavigation();

  void draw() override;
private:
  Session *session;
  MenuKeybinds *keybinds;

  SpriteAtlas *menu_atlas;
  SpriteAtlas *rest_atlas;
  SoundAtlas *sfx;

  Texture frame;
  float frame_height = 0;
  constexpr static Vector2 frame_position = {65, 36};
  float percentage = 0.0;
};
