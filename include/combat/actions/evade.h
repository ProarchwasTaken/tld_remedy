#pragma once
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "system/sprite_atlas.h"


struct EvadeSpriteSet {
  int id_windup = 0;
  int id_active = 0;
  int id_miss = 0;
  int id_evade = 0;
  int id_perfect = 0;
};


/* Evade is a defensive CombatAction that is basically the safer and 
 * slower brother of the Ghoststep action. With it's wind up time, it's
 * designed to work best against attacks that inflict Life damage; Along
 * with any attacks that would normally be harder to avoid with just 
 * Ghoststep alone. With good timing, Evade serves as a powerful reversal
 * tool that could turn the game in the player's favour.*/
class Evade : public CombatAction {
public:
  Evade(PartyMember *user, SpriteAtlas &user_atlas, RectEx hitbox,
        EvadeSpriteSet &sprite_set);
  ~Evade();

  void intercept(DamageData &data) override;

  void windUp() override;
  void action() override;
  void endLag() override;

  void drawDebug() override;

  bool evaded_attack = false;
private:
  PartyMember *user;
  RectEx hitbox;

  EvadeSpriteSet *sprite_set;
  SpriteAtlas *user_atlas;
};
