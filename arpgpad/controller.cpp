#include "stdafx.h"

#include <cassert>

#include "controller.h"

namespace {

// clang-format off
const struct {
  const char* string;
  Controller::Button button;
} kStringToButton[] = {
    {"A", Controller::kButtonA},
    {"B", Controller::kButtonB},
    {"X", Controller::kButtonX},
    {"Y", Controller::kButtonY},
    {"LB", Controller::kButtonLb},
    {"RB", Controller::kButtonRb},
    {"BACK", Controller::kButtonBack},
    {"START", Controller::kButtonStart},
    {"LSTICK", Controller::kButtonLStick},
    {"RSTICK", Controller::kButtonRStick},
    {"DPAD_UP", Controller::kButtonDpadUp},
    {"DPAD_DOWN", Controller::kButtonDpadDown},
    {"DPAD_LEFT", Controller::kButtonDpadLeft},
    {"DPAD_RIGHT", Controller::kButtonDpadRight},
};
// clang-format on

}  // namespace

// static
const char* Controller::ButtonToString(Button button) {
  for (const auto& item : kStringToButton) {
    if (button = item.button) {
      return item.string;
    }
  }

  assert(false);
  return "";
}

// static
std::optional<Controller::Button> Controller::StringToButton(
    const std::string& string) {
  for (auto& item : kStringToButton) {
    if (item.string == string) {
      return item.button;
    }
  }

  return std::nullopt;
}

std::ostream& operator<<(std::ostream& stream, const Controller::State& state) {
  stream << "\n";
  stream << "lstick: (" << state.lstick.y() << ", " << state.lstick.x()
         << ")\n";
  stream << "rstick: (" << state.rstick.y() << ", " << state.rstick.x()
         << ")\n";
  stream << "ltrigger: " << state.ltrigger << "\n";
  stream << "rtrigger: " << state.rtrigger << "\n";
  stream << "buttons: (";

  for (int button = 1; button < Controller::kButtonMaxButton; button <<= 1) {
    if (state.buttons & button) {
      stream << " "
             << Controller::ButtonToString(
                    static_cast<Controller::Button>(button));
    }
  }
  stream << ")\n";

  return stream;
}

bool operator==(const Controller::State& lhs, const Controller::State& rhs) {
  return lhs.lstick == rhs.lstick && lhs.rstick == rhs.rstick &&
         lhs.ltrigger == rhs.ltrigger && lhs.rtrigger == rhs.rtrigger &&
         lhs.buttons == rhs.buttons;
}

bool operator!=(const Controller::State& lhs, const Controller::State& rhs) {
  return !(lhs == rhs);
}
