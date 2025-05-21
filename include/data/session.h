#pragma once
#include <string>

struct Player {
  float life = 20;
  float max_life = 20;

  float init_morale = 10;
  float morale = init_morale;
  float max_morale = 35;
};


struct Session {
  Player player;
  int supplies = 0;

  std::string location = "db_01";
};
