#pragma once
#include <set>
#include <vector>
#include <string>
#include <raylib.h>
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"
#include "data/rect_ex.h"


class Projectile : public Entity {
public:
  inline static std::set<Projectile*> existing_projectiles;

  Projectile(std::string name, Combatant *owner, Vector2 position);
  ~Projectile();

  float radians(float degrees);
  float degrees(float radians);

  void launch(float velocity, float angle);
  void predictTrajectory(float accuracy);
  bool inBounds(Vector2 position);

  void drawDebug() override;

  std::string name;
  ProjectileID id;

  Combatant *owner;
  CombatantTeam alignment;

protected:
  float angle = 0;
  Vector2 direction = {0, 0};

  float velocity = 50;
  float terminal_velocity = 50;

  float drag = 0;
  float gravity = 0.0;

  RectEx hitbox;
  std::vector<Vector2> trajectory;

  static constexpr float GROUND_LEVEL = 152;
};
