#include <cassert>
#include <string>
#include <vector>
#include "enums.h"
#include "base/field_sequence.h"
#include "field/system/field_handler.h"
#include "menu/panels/dialog.h"
#include "field/sequences/rest.h"

using std::string, std::vector;


RestSequence::RestSequence() : 
  FieldSequence("Rest Sequence", SequenceID::REST)
{

}

void RestSequence::update() {
  switch (order) {
    case 0: {
      vector<string> dialog = {
        "Open the rest menu?"
      };

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog,
                                           true);
      order++;
      break;
    }
    case 1: {
      if (said_yes) {
        FieldHandler::raise<FieldEvent>(FieldEVT::OPEN_REST);
      }

      order++;
      break;
    }
    case 2: {
      end_sequence = true;
      break;
    }
  }
}

void RestSequence::dialogHandling(PromptOptions selected) {
  assert(order == 1);
  said_yes = selected == PromptOptions::YES;
}
