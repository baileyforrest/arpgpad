#include "stdafx.h"

#include "mouse.h"

#include <cassert>

// static
const char* Mouse::ButtonString(Button button) {
  switch (button) {
    case kButtonLeft:
      return "LEFT";
    case kButtonRight:
      return "RIGHT";
    case kButtonMiddle:
      return "MIDDLE";
    case kButtonX1:
      return "X1";
    case kButtonX2:
      return "X2";
  }

  assert(false);
  return "";
}
