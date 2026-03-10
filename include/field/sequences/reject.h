#pragma once
#include <vector>
#include <string>
#include "enums.h"
#include "base/field_sequence.h"


/* This sequence is meant to be initiated if the PlayerActor attempts to 
 * use a MapTransition and they don't meet the necessary conditions.*/
class RejectSequence : public FieldSequence {
public:
  RejectSequence(FlagID id);
  void update() override;
  std::vector<std::string> getRejectDialog();
private:
  FlagID rejected_flag;
};
