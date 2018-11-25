#pragma once

#include "controller_provider_xinput.h"
#include "input_handler.h"
#include "log.h"
#include "logging_controller_delegate.h"
#include "scoped_keyboard.h"
#include "scoped_mouse.h"
#include "send_input_keyboard_mouse.h"

class Runner {
 public:
  Runner();
  ~Runner();

  bool Init();
  void Poll();

 private:
  ControllerProviderXInput controller_provider_xinput_;
  SendInputKeyboardMouse keyboard_and_mouse_;
  ScopedKeyboard keyboard_;
  ScopedMouse mouse_;
  InputHandler input_handler_;
};
