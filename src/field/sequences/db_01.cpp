#include <string>
#include <vector>
#include "enums.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
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
      PLOGI << "Setting up dialog panel.";
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
      end_sequence = true;
      break;
    }
  }
}
