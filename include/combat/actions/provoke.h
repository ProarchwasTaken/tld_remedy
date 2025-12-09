#pragma once
#include "base/combat_action.h"
#include "combat/combatants/party/erwin.h"
#include "system/sprite_atlas.h"


class Provoke : public CombatAction {
public:
  Provoke(Erwin *user);
  ~Provoke();

  void drawEnemyAggro();

  void windUp() override;
  void action() override;
  void endLag() override;
private:
  Erwin *user;
  SpriteAtlas *atlas;
};
