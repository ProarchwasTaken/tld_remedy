#pragma once
#include <cstddef>
#include <raylib.h>
#include "data/session.h"
#include "data/keybinds.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/combatants/party/mary.h"


class ItemCmdHud {
public:
  ItemCmdHud(Vector2 position);
  ~ItemCmdHud();

  void enable();
  void disable();
  void assign(Mary *&player, PartyMember *&companion, 
              Session *session);

  void update();
  void draw();

  bool enabled = false;
private:
  Mary **player = NULL;
  PartyMember **companion = NULL;

  Session *session;

  Vector2 main_position;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
