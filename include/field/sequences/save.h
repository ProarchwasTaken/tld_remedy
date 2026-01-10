#pragma once
#include "base/field_sequence.h"
#include "menu/panels/dialog.h"


class SaveSequence : public FieldSequence {
public:
  SaveSequence();
  void update() override;
  void dialogHandling(PromptOptions selected) override;
private:
  bool said_yes;
};
