#pragma once
#include <memory>
#include "enums.h"
#include "base/party_member.h"
#include "base/ai_parameters.h"
#include "data/session.h"
#include "data/damage.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


enum class XanderGoals {
  IDLE = 0,
  LOOK_AT_PLR = 1,
  FOLLOW_PLR = 2,
  PROTECT_PLR = 3,
  TARGETING = 4,
  RETREATING = 5,
  TAIL_WHIP = 6,
  STEEL_WALL = 7,
  METEOR_READY = 8
};

/* The parameters that govern how Xander will behave. In this case, it's
 * intended to make Xander very slow when it comes to attacking.
 * He's waits and retreats more often, and for longer spans of 
 * time. Along with having a long contesting distance, it would take a
 * while for Xander to actually get a close enough to his target for an
 * attack. He is also very prone to disengaging from his target.*/
struct XanderAI : AIParameters {
  XanderAI() {
    attack_distance = 56;
    contest_distance = 160;

    attack_cooldown = 1.5;
    retreat_time = 0.5;
    wait_time = 0.5;

    contesting = {0.60, 0.40, 0.80, 0.60, 0.5, 1.0};
    targeting = {0.80, 0.80, 1.5};
    retreating = {0.30, 0.30, 0.75};
    damaged.retaliation_chance = 1.0;
  };
};


/* Xander is a Companion Combatant of the "Sentinel" archetype. Their
 * behavior is designed around providing meaningful backup and protection
 * to the player. While they do excel at DPS and Tanking, they are very
 * much liable to taking damage when left on their own.*/
class Xander : public PartyMember {
public:
  Xander(Companion *data, Mary *player);
  ~Xander();

  void setEnabled(bool value) override;

  void damageMorale(float magnitude) override;
  void enterHitstun(DamageData &data) override;
  bool nullifyHitstun(DamageData &data);

  void setKnockback(float velocity, float seconds, 
                    Direction direction) override;

  void behavior() override;
  void rootBehavior();

  void update() override;
  void neutralLogic();

  void followPlayer();

  void movement(float multiplier);
  void takeStep();
  void stepping(float multiplier);

  void animationLogic();
  Animation *getIdleAnim();

  void endLogic() override;

  void draw() override;
  void drawDebug() override;

  XanderGoals ai_goal = XanderGoals::IDLE;
  std::unique_ptr<AIParameters> ai;
  float tick_clock = 0.0;

  Mary *player;
  static SpriteAtlas atlas;
private:
  const float def_step_interval = 0.5;
  const float def_step_distance = 32; 
  const float default_speed = 90;

  float step_clock = 0.0;
  bool taking_step = false;
  Vector2 intended_pos;

  int moving_x = 0;
  bool has_moved = false;
  float last_moved = 1.0;

  float preferred_plr_distance = 32;

  Animation anim_idle = {{0, 0, 2, 1}, 1.5};
  Animation anim_crit = {{3, 4}, 1.0};
  Animation anim_move = {{5, 6, 7, 6}, 0.5};

  bool protective = false;
};
