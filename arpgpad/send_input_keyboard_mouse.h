#pragma once

#include "keyboard.h"
#include "mouse.h"

class SendInputKeyboardMouse : public Keyboard, public Mouse {
 public:
  SendInputKeyboardMouse();
  ~SendInputKeyboardMouse() override;

  // Keyboard implementation:
  void NotifyKeyPress(KeyCode code) override;
  void NotifyKeyRelease(KeyCode code) override;

  // Mouse implementation:
  std::pair<int, int> GetCursorPos() override;
  void SetCursorPos(int x, int y) override;
  void PressButton(Button button) override;
  void ReleaseButton(Button button) override;

 private:
};
