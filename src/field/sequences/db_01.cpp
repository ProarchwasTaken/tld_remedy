#include <cassert>
#include <string>
#include <vector>
#include "enums.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
#include "menu/panels/dialog.h"
#include "field/system/field_handler.h"
#include "field/sequences/db_01.h"
#include <plog/Log.h>

using std::vector, std::string;


DBSequence01::DBSequence01() : 
  FieldSequence("Debug Sequence #1", SequenceID::DB_01)
{

}

void DBSequence01::update() {
  switch (order) {
    case 0: {
      vector<string> dialog = {
        "Hello!",
        "This is test dialogue!",
        "It even supports multiple lines as you\n"
        "can see here."
      };

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG,
                                           dialog);
      order++;
      break;
    }
    case 1: {
      vector<string> dialog = {"Isn't this pretty cool?"};

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG,
                                           dialog, true);
      order++;
      break;
    }
    case 2: {
      vector<string> dialog;

      if (said_yes) {
        dialog = {"Glad to hear that!"};
      }
      else {
        dialog = {"Alright then..."};
      }

      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG,
                                           dialog);
      order++;
      break;
    }
    case 3: {
      end_sequence = true;
      break;
    }
  }
}

void DBSequence01::dialogHandling(PromptOptions selected) {
  assert(order == 2);
  said_yes = selected == PromptOptions::YES;
}
