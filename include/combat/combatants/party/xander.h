#pragma once
#include "base/party_member.h"
#include "data/session.h"
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
  void draw() override;

  Mary *player;
  static SpriteAtlas atlas;
};
