#pragma once
#include "enums.h"

#define STATUS_LIMIT 3
#define ITEM_LIMIT 8


struct Character {
  char name[9];
  PartyMemberID member_id;

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

struct Player : Character {
  SubWeaponID weapon_id;
};

struct Companion : Character {
  CompanionID companion_id;
};

struct CommonData {
  int object_id = -1;
  bool active = false;

  char map_name[16] = "";
};

struct Session {
  unsigned int version;
  double playtime = 0.0;
  char location[16] = "db_05";

  Player player;
  Companion companion;
  
  int supplies = 0;

  int item_count = 2;
  int item_limit = ITEM_LIMIT;
  ItemID inventory[8] = {
    ItemID::I_BANDAGE,
    ItemID::M_SPLINT,
    ItemID::NONE,
    ItemID::NONE,
    ItemID::NONE,
    ItemID::NONE,
    ItemID::NONE
  };

  int common_count = 0;
  int common_limit = 64;
  CommonData common[64];
};
