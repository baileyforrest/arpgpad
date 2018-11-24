#pragma once

#include "mouse.h"
#include "scoped_destructor.h"

#include <map>

class ScopedMouse {
 public:
  ScopedMouse(Mouse* mouse) : mouse_(mouse) {}
  ~ScopedMouse();

  void SetCursorPos(int x, int y) { mouse_->SetCursorPos(x, y); }
  ScopedDestructor GetMousePressToken(Mouse::Button button);

 private:
  Mouse* const mouse_;
  std::map<Mouse::Button, int> button_to_press_count_;
};
