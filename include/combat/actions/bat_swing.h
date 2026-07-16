#pragma once
#include <utility>
#include <vector>
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class BatSwing : public CombatAction {
public:
  BatSwing(Mary *user);
  ~BatSwing();

  void intercept(DamageData &data) override;
  void flawedClash(DamageData &data);
  void normalClash(DamageData &data);

  void updateHitboxOffset();
  void sendWarning();

  void windUp() override;
  void freeTurning();

  void action() override;
  void hitRegistration(std::vector<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::vector<std::pair<float, Combatant*>> &hits);

  void endLag() override;

  void drawDebug() override;
private:
  Mary *user;
  RectEx hitbox;
  DamageData data;

  const int attack_weight = 2;
  bool attack_connected = false;
  bool clashed = false;

  SpriteAtlas *atlas;
  Animation anim_windup = {{38, 39, 40}, 0.2};
  Animation anim_end = {{42, 43}, 0.3};
};
