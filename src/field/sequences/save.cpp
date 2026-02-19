#include <cassert>
#include <string>
#include <vector>
#include "enums.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
#include "field/system/field_handler.h"
#include "menu/panels/dialog.h"
#include "field/sequences/save.h"

using std::string, std::vector;


SaveSequence::SaveSequence() : 
  FieldSequence("Save Sequence", SequenceID::SAVE)
{

}

void SaveSequence::update() {
  switch (order) { 
    case 0: {
      vector<string> dialog = {
        "The air here doesn't feel as tense\n"
        "as anywhere else. Might be a good\n"
        "spot to take a breather.",
        "Record your progress?\n"
        "(Existing data will be overwritten.)"
      };

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog,
                                           true);
      order++;
      break;
    }
    case 1: {
      if (said_yes) {
        vector<string> dialog = {"Game saved."};
        FieldHandler::raise<FieldEvent>(FieldEVT::SAVE_SESSION);
        FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, 
                                             dialog);
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

void SaveSequence::dialogHandling(PromptOptions selected) {
  assert(order == 1);
  said_yes = selected == PromptOptions::YES;
}

