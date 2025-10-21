#pragma once
#include <raylib.h>
#include "data/session.h"
#include "data/keybinds.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"


class ItemCmdHud {
public:
  ItemCmdHud(Vector2 position, Session *session);
  ~ItemCmdHud();

  void enable();
  void disable();
  void assign(PartyMember *player, PartyMember *companion);

  void update();
  void draw();

  bool enabled = false;
private:
  PartyMember **player;
  PartyMember **companion;

  Session *session;

  Vector2 main_position;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
};
