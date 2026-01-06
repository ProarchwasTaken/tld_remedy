#pragma once
#include "base/field_sequence.h"


class DBSequence01 : public FieldSequence {
public:
  DBSequence01();
  void update() override;
};
