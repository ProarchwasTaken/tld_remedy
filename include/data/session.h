#pragma once
#include "enums.h"


struct Player {
  char name[9] = "Mary";
  SubWeaponID sub_weapon = SubWeaponID::KNIFE;

  float life = 15;
  float max_life = 15;

  float init_morale = 10;
  float max_morale = 30;

  int offense = 6;
  int defense = 6;
  int intimid = 6;
  int persist = 6;
};

struct Companion {
  char name[9];
  CompanionID id;

  float life;
  float max_life;

  float init_morale;
  float max_morale;

  int offense;
  int defense;
  int intimid;
  int persist;
};

struct CommonData {
  int object_id = -1;
  bool active = false;

  char map_name[16] = "";
};

struct Session {
  unsigned int version;
  char location[16] = "db_01";

  Player player;
  Companion companion;
  
  int supplies = 0;

  int common_count = 0;
  int common_limit = 64;
  CommonData common[64];
};
