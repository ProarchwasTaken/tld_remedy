#pragma once
#include "base/field_sequence.h"


class DSErwinMild: public FieldSequence {
public:
  DSErwinMild();
  void setup();

  void update() override;
  void draw() override;
private:
  float seq_clock = 0.0;
  float seq_time = 0.25;

  bool black_out = true;
};
