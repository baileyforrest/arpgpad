#pragma once

#include <iostream>

class Mouse {
 public:
  enum Button {
    kButtonLeft,
    kButtonRight,
    kButtonMiddle,

    kButtonXStart,
    kButtonX1 = kButtonXStart,  // Usually Back
    kButtonX2,                  // Usually Forward

    // TODO: Do other button like X3+ work?

    kButtonNumTypes,
  };

  static const char* ButtonString(Button button);

  virtual std::pair<int, int> GetCursorPos() = 0;
  virtual void SetCursorPos(int x, int y) = 0;

  virtual void PressButton(Button button) = 0;
  virtual void ReleaseButton(Button button) = 0;
};

inline std::ostream& operator<<(std::ostream& os, Mouse::Button button) {
  os << Mouse::ButtonString(button);
  return os;
}
