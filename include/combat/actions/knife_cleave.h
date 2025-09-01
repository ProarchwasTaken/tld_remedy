#pragma once
#include <utility>
#include <set>
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class KnifeCleave : public CombatAction {
public:
  KnifeCleave(Mary *user);
  ~KnifeCleave();

  bool cancelCheck();
  void updateAnimFrameDuration();

  void windUp() override;
  void action() override;
  void hitRegistration(std::set<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::set<std::pair<float, Combatant*>> &hits);

  void endLag() override;

  void drawDebug() override;
private:
  RectEx hitbox;
  DamageData data;

  bool canceled_into;
  bool attack_connected = false;

  SpriteAtlas *atlas;
  Animation anim_end = {{22, 23}, 0.0};
};
