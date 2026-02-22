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
        "A rare, but valuable blind spot hidden\n"
        "away from the prying eyes of this\n"
        "hostile \"reality\".",
        "Would you like to rest?"
      };

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog,
                                           true);
      order++;
      break;
    }
    case 1: {
      if (said_yes) {
        FieldHandler::raise<FieldEvent>(FieldEVT::OPEN_REST);
        FieldHandler::raise<FieldEvent>(FieldEVT::REVIVE_ENEMIES);
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
