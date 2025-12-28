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
    };
  }
};

struct DBTroop2 : EnemyTroop {
  DBTroop2() {
    id = TroopID::DB_TROOP2;
    enemies = {
      {EnemyID::DUMMY, {32, 152}, LEFT},
      {EnemyID::DUMMY, {64, 152}, LEFT}
    };
  }
};

struct DBTroop3 : EnemyTroop {
  DBTroop3() {
    id = TroopID::DB_TROOP3;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
      {EnemyID::SERVANT, {160, 152}, LEFT},
      // {EnemyID::SERVANT, {-180, 152}, RIGHT},
      // {EnemyID::SERVANT, {-270, 152}, RIGHT},
    };
  }
};
