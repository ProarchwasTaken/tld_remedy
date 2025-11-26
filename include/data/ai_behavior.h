#pragma once


struct AIBehavior {
  float ct_retreat_chance;
  float ct_min_retreat;
  float ct_max_retreat;

  float ct_wait_chance;
  float ct_min_wait;
  float ct_max_wait;

  float tg_retreat_chance;
  float tg_min_retreat;
  float tg_max_retreat; 

  float rt_target_chance;
  float rt_min_wait;
  float rt_max_wait;

  float dg_target_chance;
  float dg_min_wait;
  float dg_max_wait;

  float dg_range_multiplier;
  float dg_time_multiplier;
  float dg_penalty;
  float dg_retailiation_chance;
};

