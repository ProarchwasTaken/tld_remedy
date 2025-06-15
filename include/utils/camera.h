#pragma once
#include <raylib.h>
#include "base/entity.h"
#include "combat/combatants/player.h"

namespace CameraUtils {
Camera2D setupField();
Camera2D setupCombat();
void followFieldEntity(Camera2D &camera, Entity *entity);

void combatCamera(Camera2D &camera, PlayerCombatant *player);
void enemyTarget(Camera2D &camera, PlayerCombatant *player);
void combatFollow(Camera2D &camera, float x);
}
