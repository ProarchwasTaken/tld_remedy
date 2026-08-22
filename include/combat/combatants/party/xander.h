#pragma once
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


/* Xander is a Companion Combatant of the "Sentinel" archetype. Their
 * behavior is designed around providing meaningful backup and protection
 * to the player. While they do excel at DPS and Tanking, they are very
 * much liable to taking damage when left on their own.*/
class Xander : public PartyMember {
public:
  Xander(Companion *data, Mary *player);
  ~Xander();

  void update() override;

  void animationLogic();
  Animation *getIdleAnim();

  void draw() override;

  Mary *player;
  static SpriteAtlas atlas;
private:
  Animation anim_idle = {{0, 0, 0, 2, 1}, 1.5};
  Animation anim_crit = {{3, 4}, 1.0};
};
