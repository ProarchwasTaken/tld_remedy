#pragma once


struct Contesting {
  float retreat_chance;
  float min_retreat;
  float max_retreat;

  float wait_chance;
  float min_wait; 
  float max_wait;
};

struct Targeting {
  float retreat_chance;
  float min_retreat;
  float max_retreat;
};

struct Retreating {
  float target_chance;
  float min_wait;
  float max_wait;
};

struct Dodging {
  float target_chance;
  float min_wait;
  float max_wait;

  float range_multiplier;
  float time_multiplier;
  float penalty;
  float retaliation_chance;
};

struct Damaged {
  float retreat_chance;
  float min_retreat;
  float max_retreat;

  float retaliation_chance;
};


/* The parameters that govern the behavior of every non-playable 
 * Combatant; Enemy or otherwise. There are 4 core traits that every
 * Combatant AI shares: Contesting, Targeting, Retreating, and Dodging.
 * The parameters that relate to any of these can be freely changed.*/
struct AIParameters {
  float attack_distance;
  float contest_distance;

  float attack_cooldown;
  float cooldown_clock = 1.0;

  float retreat_time;
  float retreat_clock = 1.0;

  float dodge_time = 0.0;
  float dodge_clock = 0.0;

  bool waiting = false;
  float wait_time;
  float wait_clock = 0.0;

  Contesting contesting;
  Targeting targeting;
  Retreating retreating;
  Dodging dodging;
  Damaged damaged;
};

