#include "stdafx.h"

#include "mouse.h"

#include <cassert>

namespace {

// clang-format off
const struct {
  const char* string;
  Mouse::Button button;
} kStringToButton[] = {
    { "LEFT", Mouse::kButtonLeft},
    { "RIGHT", Mouse::kButtonRight},
    { "MIDDLE", Mouse::kButtonMiddle},
    { "X1", Mouse::kButtonX1},
    { "X2", Mouse::kButtonX2},
};
// clang-format on

}  // namespace

// static
const char* Mouse::ButtonToString(Button button) {
  for (const auto& item : kStringToButton) {
    if (button = item.button) {
      return item.string;
    }
  }

  assert(false);
  return "";
}

// static
std::optional<Mouse::Button> Mouse::StringToButton(const std::string& string) {
  for (auto& item : kStringToButton) {
    if (item.string == string) {
      return item.button;
    }
  }

  return std::nullopt;
}
