#include "stdafx.h"

#include "scoped_mouse.h"

#include <cassert>

ScopedMouse::~ScopedMouse() { assert(button_to_press_count_.empty()); }

ScopedDestructor ScopedMouse::GetMousePressToken(Mouse::Button button) {
  if (++button_to_press_count_[button] == 1) {
    mouse_->PressButton(button);
  }
  return ScopedDestructor([this, button] {
    auto it = button_to_press_count_.find(button);
    assert(it != button_to_press_count_.end());
    if (--it->second == 0) {
      mouse_->ReleaseButton(button);
      button_to_press_count_.erase(it);
    }
  });
}
