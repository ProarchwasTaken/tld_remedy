#pragma once
#include <utility>
#include <vector>
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "data/animation.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/xander.h"


class TailWhip : public CombatAction {
public:
  TailWhip(Xander *user);
  ~TailWhip();

  void initBodyData();
  void initWhipData();
  void sendWarning();

  void windUp() override;
  void bodyCheck();
  void lungeMovement();

  void action() override;
  void whipHitReg(std::vector<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::vector<std::pair<float, Combatant*>> &hits);

  void endLag() override;

  void drawDebug() override;
private:
  RectEx whip_hitbox;
  RectEx body_hitbox;

  int atk;
  DamageData data;
  const int attack_weight = 3;

  bool performed_check = false;
  bool attack_connected = false;

  Xander *user;
  SpriteAtlas *atlas;
  SoundAtlas *sfx;

  Animation anim_windup = {{15, 16, 17}, 0.175};
  Animation anim_end = {{19, 20}, 0.10};
};
