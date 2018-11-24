#pragma once

#include <map>

#include "keyboard.h"
#include "scoped_destructor.h"

class ScopedKeyboard {
 public:
  ScopedKeyboard(Keyboard* keyboard) : keyboard_(keyboard) {}
  ~ScopedKeyboard();

  ScopedDestructor GetKeyPressToken(Keyboard::KeyCode code);

 private:
  Keyboard* const keyboard_;
  std::map<Keyboard::KeyCode, int> key_to_press_count_;
};
