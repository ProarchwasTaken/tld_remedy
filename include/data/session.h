#pragma once


struct Player {
  float life = 20;
  float max_life = 20;

  float init_morale = 10;
  float max_morale = 35;
};

struct CommonData {
  int object_id = -1;
  bool active = false;

  char map_name[16];
};

struct Session {
  Player player;
  int supplies = 0;

  char location[16] = "db_01";

  int common_count = 0;
  int common_limit = 64;
  CommonData common[64];
};
