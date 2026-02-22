#pragma once
#include "base/field_sequence.h"
#include "menu/panels/dialog.h"


class RestSequence : public FieldSequence {
public:
  RestSequence();
  void update() override;
  void dialogHandling(PromptOptions selected) override;
private:
  bool said_yes;
};
