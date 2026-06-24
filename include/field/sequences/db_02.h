#pragma once
#include "base/field_sequence.h"


class DBSequence02 : public FieldSequence {
public:
  DBSequence02();
  void update() override;
};
