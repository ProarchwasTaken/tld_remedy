#pragma once
#include <set>
#include <vector>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "data/rect_ex.h"

struct TrajectPoint {
  Vector2 position;
  Vector2 direction;
  float velocity;
  float seconds;
};

struct OncomingCollision {
  Combatant *target;
  TrajectPoint *point;
  Rectangle intersect;
  bool warned = false;
};


class Projectile : public Entity {
public:
  inline static std::set<Projectile*> existing_projectiles;

  Projectile(std::string name, Combatant *owner, Vector2 position);
  ~Projectile();

  float radians(float degrees);
  float degrees(float radians);
  bool inBounds(Vector2 position);

  void launch(float velocity, float angle);
  void predictTrajectory(float interval);

  void detectOncoming();
  int inTrajectory(Combatant *combatant);

  /* Since projectiles are entities that operate independent from the
   * owner, we must acknowledge the possibility of the owner dying 
   * while the projectile is still alive. So this function is here to
   * automatically set the owner pointer to NULL as soon as it detects
   * that the owner is dead.*/
  void ownerCheck();
  void runPhysics();

  void warningProcess();

  void lifeTimer();
  virtual void onEndLife();

  void deathTimer();

  void drawSprite(Texture *sheet);
  void applyFlicker(float x, Color &tint);

  void drawDebug() override;
  void drawOncoming();

  std::string name;
  ProjectileID id;
  bool dying = false;

  Combatant *owner;
  CombatantTeam alignment;

  Color tint = WHITE;
protected:
  float angle = 0;
  Vector2 direction = {0, 0};

  float velocity = 50;
  float terminal_velocity = 50;

  float drag = 0;
  float gravity = 0.0;

  RectEx hitbox;
  std::vector<TrajectPoint> trajectory;

  std::vector<OncomingCollision> oncoming_collisions;
  int max_collisions = 99;
  float warning_time = 0.5;

  float life_time = 1.0;
  float life_clock = -1;
  float max_life_time = 60; 

  float death_time = 0.5;
  float death_clock = 0.0;

  static constexpr float GROUND_LEVEL = 152;
};
