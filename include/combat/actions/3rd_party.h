#pragma once
#include <utility>
#include <set>
#include "base/combat_action.h"
#include "data/animation.h"
#include "data/damage.h"
#include "data/rect_ex.h"
#include "combat/combatants/party/erwin.h"
#include "system/sprite_atlas.h"


class ThirdParty : public CombatAction {
public:
  ThirdParty(Erwin *user);
  ~ThirdParty();

  void sendWarning();

  void windUp() override;
  void action() override;
  void hitRegistration(std::set<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::set<std::pair<float, Combatant*>> &hits);

  void endLag() override;

  void drawDebug() override;
private:
  Erwin *user;
  RectEx hitbox;
  DamageData data;

  bool attack_connected = false;;

  SpriteAtlas *atlas;
  Animation anim_windup = {{25, 26}, 0.15};
  Animation anim_end = {{28, 29}, 0.40};
};
