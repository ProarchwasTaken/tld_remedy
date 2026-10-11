#pragma once
#include <utility>
#include <set>
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/combatants/party/xander.h"


class Meteor : public CombatAction {
public:
  Meteor(Xander *user);
  ~Meteor();
  void sendWarning();

  void windUp() override;

  void action() override;
  void hitRegistration(std::set<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::set<std::pair<float, Combatant*>> &hits);
  void attemptBoneBreak(Combatant *victim);

  void endLag() override;

  void drawDebug() override;
private:
  DamageData data;
  bool attack_connected = false;

  RectEx hitbox;
  RectEx warning_hitbox;

  Xander *user;
  SpriteAtlas *atlas;
  SoundAtlas *sfx;

  Animation anim_windup = {{22, 23, 24, 25}, 0.30};
  Animation anim_end = {{28, 29}, 0.75};
};
