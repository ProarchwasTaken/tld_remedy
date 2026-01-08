#pragma once
#include "base/field_sequence.h"
#include "menu/panels/dialog.h"


class DBSequence01 : public FieldSequence {
public:
  DBSequence01();
  void update() override;
  void dialogHandling(PromptOptions selected) override;
private:
  bool said_yes;
};
