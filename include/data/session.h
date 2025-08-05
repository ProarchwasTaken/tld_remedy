#pragma once
#include "enums.h"

#define STATUS_LIMIT 3

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

  int status_count = 0;
  int status_limit = STATUS_LIMIT;
  StatusID status[3] = {StatusID::NONE, StatusID::NONE, StatusID::NONE};
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

  int status_count = 0;
  int status_limit = STATUS_LIMIT;
  StatusID status[3] = {StatusID::NONE, StatusID::NONE, StatusID::NONE};
};

struct CommonData {
  int object_id = -1;
  bool active = false;

  char map_name[16] = "";
};

struct Session {
  unsigned int version;
  char location[16] = "db_05";

  Player player;
  Companion companion;
  
  int supplies = 0;

  int common_count = 0;
  int common_limit = 64;
  CommonData common[64];
};
