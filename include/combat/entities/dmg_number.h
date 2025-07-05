#include <raylib.h>
#include "enums.h"
#include "base/entity.h"
#include "base/combatant.h"


class DamageNumber : public Entity {
public:
  DamageNumber(Combatant *target, DamageType damage_type, float value);
  void setStartPosition(Combatant *target);
  void setTextColor(DamageType damage_type);

  void update() override;
  void draw() override;
private:
  bool visible = false;
  float value;
  Color color;

  float initial_speed = 64;
  float life_time = 0.5;
  float life_clock = 0.0;
};
