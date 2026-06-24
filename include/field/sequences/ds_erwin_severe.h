#pragma once
#include "base/field_sequence.h"


class DSErwinSevere : public FieldSequence {
public:
  DSErwinSevere();
  void setup();

  void update() override;
  void draw() override;
private:
  float seq_clock = 0.0;
  float seq_time = 0.50;

  bool black_out = true;
};
