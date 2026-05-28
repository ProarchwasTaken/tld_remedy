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
  CRAFT_FAKIT,
  RECYCLE_ITEM,
  MOVE_ITEM
};


class CraftingPanel : public Panel {
public:
  CraftingPanel(Session *session, SpriteAtlas *rest_atlas);
  ~CraftingPanel();

  void update() override;
  void heightLerp();
  void panelLogic();
  void promptHandling();

  void slotSelection();
  void optionSelection();
  void swapSlotSelection();

  void selectOption();
  void openCraftingDialog(ItemID item);
  void openRecycleDialog(ItemID item);
  void enterSwapMode();

  void craftItem();
  void recycleItem();
  void swapItemSlots();

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
  int getSupplyRefund(ItemID id);
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
  std::array<ItemID, 8>::iterator swap_slot;
  float blink_clock = 0.0;

  bool craft_mode = false;
  bool swap_mode = false;
  std::array<CraftOptions, 8> options = {
    CRAFT_IBANDAGE,
    CRAFT_SBANDAGE,
    CRAFT_FAKIT,
    CRAFT_MSPLINT,
    CRAFT_PKILLERS,
    CRAFT_SWATER,
    RECYCLE_ITEM,
    MOVE_ITEM
  };
  std::array<CraftOptions, 8>::iterator selected_option = options.begin();

  std::unique_ptr<DialogPanel> panel;
  bool panel_mode = false;
};
