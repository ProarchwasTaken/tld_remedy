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

struct CDTroop1 : EnemyTroop {
  CDTroop1() {
    id = TroopID::CD_TROOP1;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
    };
  }
};

struct CDTroop2 : EnemyTroop {
  CDTroop2() {
    id = TroopID::CD_TROOP2;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
      {EnemyID::SERVANT, {160, 152}, LEFT},
    };
  }
};

struct CDTroop3 : EnemyTroop {
  CDTroop3() {
    id = TroopID::CD_TROOP3;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
      {EnemyID::SERVANT, {-180, 152}, RIGHT},
    };
  }
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
    };
  }
};

struct DBTroop4 : EnemyTroop {
  DBTroop4()  {
    id = TroopID::DB_TROOP4;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
      {EnemyID::SERVANT, {160, 152}, LEFT},
    };
  }
};

struct DBTroop5 : EnemyTroop {
  DBTroop5() {
    id = TroopID::DB_TROOP5;
    enemies = {
      {EnemyID::SERVANT, {32, 152}, LEFT},
      {EnemyID::SERVANT, {160, 152}, LEFT},
      {EnemyID::SERVANT, {-180, 152}, RIGHT},
    };
  }
};
