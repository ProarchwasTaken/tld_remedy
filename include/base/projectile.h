#pragma once
#include <set>
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

  void launch(float velocity, float angle);

  std::string name;
  Combatant *owner;
  CombatantTeam alignment;
protected:
  Vector2 direction = {0, 0};
  float velocity = 0;
  float terminal_velocity = 0;

  float drag = 0;
  float gravity = 0;

  RectEx hitbox;
};
