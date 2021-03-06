#include "stdafx.h"

#include "send_input_keyboard_mouse.h"

#include "log.h"

// Used for debugging without actually sending input.
#if 0
#define SendInputImpl(a0, a1, a2) \
  do {                            \
    (void)a0;                     \
    (void)a1;                     \
    (void)a2;                     \
  } while (0)
#else
#define SendInputImpl(a0, a1, a2) SendInput(a0, a1, a2)
#endif

SendInputKeyboardMouse::SendInputKeyboardMouse() = default;
SendInputKeyboardMouse::~SendInputKeyboardMouse() = default;

void SendInputKeyboardMouse::NotifyKeyPress(KeyCode code) {
  LOG(INFO) << __func__ << "(" << KeyCodeToString(code) << ")";
  INPUT input{};
  input.type = INPUT_KEYBOARD;

  input.ki.wVk = code;
  SendInputImpl(1, &input, sizeof(input));
}

void SendInputKeyboardMouse::NotifyKeyRelease(KeyCode code) {
  LOG(INFO) << __func__ << "(" << KeyCodeToString(code) << ")";
  INPUT input{};
  input.type = INPUT_KEYBOARD,

  input.ki.wVk = code;
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInputImpl(1, &input, sizeof(input));
}

std::pair<int, int> SendInputKeyboardMouse::GetCursorPos() {
  POINT point;
  if (!::GetCursorPos(&point)) {
    LOG(ERR) << "Failed to get cursor position: " << GetLastError();
    return {0, 0};
  }

  return std::pair<int, int>(point.x, point.y);
}

void SendInputKeyboardMouse::SetCursorPos(int x, int y) {
  LOG(INFO) << __func__ << "(" << x << ", " << y << ")";
  if (!::SetCursorPos(x, y)) {
    LOG(ERR) << __func__ << " failed: " << GetLastError();
  }
}

void SendInputKeyboardMouse::PressButton(Button button) {
  LOG(INFO) << __func__ << "(" << button << ")";

  INPUT input{};
  input.type = INPUT_MOUSE;

  switch (button) {
    case kButtonLeft:
      input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
      break;
    case kButtonRight:
      input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
      break;
    case kButtonMiddle:
      input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
      break;
    case kButtonX1:
    case kButtonX2:
      input.mi.dwFlags = MOUSEEVENTF_XDOWN;
      input.mi.mouseData = button - kButtonXStart + 1;
      break;
  }
  SendInputImpl(1, &input, sizeof(input));
}

void SendInputKeyboardMouse::ReleaseButton(Button button) {
  LOG(INFO) << __func__ << "(" << button << ")";
  INPUT input{};
  input.type = INPUT_MOUSE;

  switch (button) {
    case kButtonLeft:
      input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
      break;
    case kButtonRight:
      input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
      break;
    case kButtonMiddle:
      input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
      break;
    case kButtonX1:
    case kButtonX2:
      input.mi.dwFlags = MOUSEEVENTF_XUP;
      input.mi.mouseData = button - kButtonXStart + 1;
      break;
  }
  SendInputImpl(1, &input, sizeof(input));
}
