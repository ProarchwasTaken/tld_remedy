#include <array>
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "menu/panels/dialog.h"

enum CraftOptions {
  CRAFT_IBANDAGE,
  CRAFT_MSPLINT,
  CRAFT_SBANDAGE,
  CRAFT_SWATER,
  CRAFT_PKILLERS,
  RECYCLE_ITEM,
  SWAP_ITEM
};


class CraftingPanel : public Panel {
public:
  CraftingPanel(Session *session, SpriteAtlas *rest_atlas);
  ~CraftingPanel();

  void update() override;
  void heightLerp();
  void panelLogic();
  void slotSelection();
  void optionSelection();

  void selectOption();
  void openCraftingDialog(ItemID item);
  void craftItem();

  void draw() override;
  void drawSupplyCount();
  void drawHelpText();
  void drawInventory();
  void drawOptions();

  void drawMiscOption(Font *font, int txt_size, Vector2 position,
                      CraftOptions option);

  void drawCraftOption(Font *font, int txt_size, Vector2 position, 
                       ItemID item);
  void drawSupplyCost(Font *font, int txt_size, Vector2 position, 
                      Color color, int cost);
  void drawCursor(Vector2 position, Color color, bool blink);

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
  std::array<CraftOptions, 7> options = {
    CRAFT_IBANDAGE,
    CRAFT_MSPLINT,
    CRAFT_SBANDAGE,
    CRAFT_SWATER,
    CRAFT_PKILLERS,
    RECYCLE_ITEM,
    SWAP_ITEM
  };
  std::array<CraftOptions, 7>::iterator selected_option = options.begin();

  std::unique_ptr<DialogPanel> panel;
  bool panel_mode = false;
};
