#pragma once
#include <utility>
#include <vector>
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"
#include "data/animation.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/xander.h"


class HandBlade : public CombatAction {
public:
  HandBlade(Xander *user);
  ~HandBlade();

  void sendWarning();

  void windUp() override;

  void action() override;
  void hitRegistration(std::vector<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::vector<std::pair<float, Combatant*>> &hits);

  void endLag() override;

  void drawDebug() override;
private:
  RectEx hitbox;
  DamageData data;

  const int attack_weight = 2;
  bool attack_connected = false;

  SpriteAtlas *atlas;
  SoundAtlas *sfx;
  Animation anim_windup = {{9, 10}, 0.25};
  Animation anim_end = {{12, 13, 14}, 0.15};
};
