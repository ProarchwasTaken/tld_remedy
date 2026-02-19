#include <array>
#include <string>
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/panels/dialog.h"


class CraftingPanel : public Panel {
public:
  CraftingPanel(Session *session, SpriteAtlas *rest_atlas);
  ~CraftingPanel();

  void update() override;
  void heightLerp();
  void panelLogic();
  void slotSelection();
  void itemSelection();

  void openCraftingDialog();
  void craftItem();

  void draw() override;
  void drawSupplyCount();
  void drawHelpText();
  void drawInventory();
  void drawOptions();
  void drawSupplyCost(Font *font, int txt_size, Vector2 position, 
                      Color color, int cost);
  void drawCursor(Vector2 position, Color color, bool blink);

  std::string getShortenedName(ItemID id);
  std::string getName(ItemID id);
  int getSupplyCost(ItemID id);
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

  std::array<ItemID, 8> inventory;
  std::array<ItemID, 8>::iterator selected_slot;
  float blink_clock = 0.0;

  bool craft_mode = false;
  std::array<ItemID, 5> craft_options = {
    ItemID::I_BANDAGE,
    ItemID::M_SPLINT,
    ItemID::S_BANDAGE,
    ItemID::S_WATER,
    ItemID::P_KILLERS
  };
  std::array<ItemID, 5>::iterator selected_option = craft_options.begin();

  std::unique_ptr<DialogPanel> panel;
  bool panel_mode = false;
};
