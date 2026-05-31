#include <string>
#include <vector>
#include "enums.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
#include "field/system/field_handler.h"
#include "field/sequences/db_02.h"
#include <plog/Log.h>

using std::vector, std::string;


DBSequence02::DBSequence02() : 
  FieldSequence("Debug Sequence #2", SequenceID::DB_02)
{

}

void DBSequence02::update() {
  switch (order) {
    case 0: {
      vector<string> dialog = {
        "C'mon, Erwin. We must overcome\n"
        "True Human Tribulation if we want to\n"
        "succeed!"
      };

      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Mary", "Martyr", dialog);
      order++;
      break;
    }
    case 1: {
      end_sequence = true;
      break;
    }
  }
}
