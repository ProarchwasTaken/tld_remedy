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
};


/* DO NOT add any more variables to this struct. For some reason it 
 * causes weird things to happen when you do, and I have no idea why.*/
struct AIBehavior {
  Contesting contesting;
  Targeting targeting;
  Retreating retreating;
  Dodging dodging;

  float retaliation_chance;
};

