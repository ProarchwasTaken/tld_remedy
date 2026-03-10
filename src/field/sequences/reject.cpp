#include <string>
#include <vector>
#include "base/actor.h"
#include "enums.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
#include "field/system/field_handler.h"
#include "field/sequences/reject.h"
#include <plog/Log.h>

using std::string, std::vector;


RejectSequence::RejectSequence(FlagID id) : 
  FieldSequence("Reject Sequence", SequenceID::REJECT)
{
  rejected_flag = id;
}

void RejectSequence::update() {
  switch (order) {
    case 0: {
      vector<string> dialog = getRejectDialog();
      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG,
                                           dialog);

      order++;
      break;
    }
    case 1: {
      int flipped = player->direction * -1;
      Direction target_direction = static_cast<Direction>(flipped);
      Vector2 target_position = player->position;

      if (flipped == DOWN || flipped== UP) {
        int direction_y = flipped / 2;
        target_position.y += 32 * direction_y;
      }
      else {
        target_position.x += 32 * flipped;
      }

      MovePoint point = {target_position, target_direction};
      player->move_points.push_front(point);

      order++;
      break;
    }
    case 2: {
      if (player->move_points.empty()) {
        order++;
      }

      break;
    }
    case 3: {
      end_sequence = true;
      break;
    }
  }
}

vector<string> RejectSequence::getRejectDialog() {
  vector<string> dialog;

  switch (rejected_flag) {
    case FlagID::CDF1_KEY: {
      dialog = {
        "The door can't seem to budge when\n"
        "Mary tries to open it.",
        "There must be a key to unlock it\n"
        "somewhere around here..."
      };
      break;
    }
    default: {
      PLOGE << "Dialog unavailiable for this flag!";
    }
  }

  return dialog;
}
