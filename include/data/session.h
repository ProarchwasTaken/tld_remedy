#pragma once

struct Player {
  float life = 20;
  float max_life = 20;

  float init_morale = 10;
  float max_morale = 35;
};


struct Session {
  Player player;
  int supplies = 0;

  char location[20] = "db_01";
};
