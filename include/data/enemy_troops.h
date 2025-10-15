#include <vector>
#include <raylib.h>
#include "enums.h"


struct EnemyData {
  EnemyID enemy;
  Vector2 position;
  Direction direction;
};

struct EnemyTroop {
  TroopID id = TroopID::INVALID;
  std::vector<EnemyData> enemies;
};

struct DBTroop1 : EnemyTroop {
  DBTroop1() {
    id = TroopID::DB_TROOP1;
    enemies = {
      {EnemyID::DUMMY, {128, 152}, LEFT},
      {EnemyID::DUMMY, {-256, 152}, RIGHT},
      {EnemyID::DUMMY, {-288, 152}, RIGHT}
    };
  }
};
