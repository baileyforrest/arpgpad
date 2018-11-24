#include "stdafx.h"

#include "scoped_keyboard.h"

#include <cassert>

ScopedKeyboard::~ScopedKeyboard() { assert(key_to_press_count_.empty()); }

ScopedDestructor ScopedKeyboard::GetKeyPressToken(Keyboard::KeyCode code) {
  if (++key_to_press_count_[code] == 1) {
    keyboard_->NotifyKeyPress(code);
  }
  return ScopedDestructor([this, code] {
    auto it = key_to_press_count_.find(code);
    assert(it != key_to_press_count_.end());
    if (--it->second == 0) {
      keyboard_->NotifyKeyRelease(code);
      key_to_press_count_.erase(it);
    }
  });
}
