#include "stdafx.h"

#include <cassert>

#include "controller.h"

// static
const char* Controller::ButtonString(Button b) {
  switch (b) {
    case kButtonA:
      return "A";
    case kButtonB:
      return "B";
    case kButtonX:
      return "X";
    case kButtonY:
      return "Y";
    case kButtonLb:
      return "LB";
    case kButtonRb:
      return "RB";
    case kButtonBack:
      return "BACK";
    case kButtonStart:
      return "START";
    case kButtonLStick:
      return "LSTICK";
    case kButtonRStick:
      return "RSTICK";
    case kButtonDpadUp:
      return "DPAD_UP";
    case kButtonDpadDown:
      return "DPAD_DOWN";
    case kButtonDpadLeft:
      return "DPAD_LEFT";
    case kButtonDpadRight:
      return "DPAD_RIGHT";
  }
  assert(false);
  return "";
}

std::ostream& operator<<(std::ostream& stream,
                          const Controller::State& state) {
  stream << "\n";
  stream << "lstick: (" << state.lstick.y() << ", " << state.lstick.x() << ")\n";
  stream << "rstick: (" << state.rstick.y() << ", " << state.rstick.x() << ")\n";
  stream << "ltrigger: " << state.ltrigger << "\n";
  stream << "rtrigger: " << state.rtrigger << "\n";
  stream << "buttons: (";

  for (int button = 1; button < Controller::kButtonMaxButton; button <<= 1) {
    if (state.buttons & button) {
      stream << " " << Controller::ButtonString(static_cast<Controller::Button>(button));
    }
  }
  stream << ")\n";

  return stream;
}
