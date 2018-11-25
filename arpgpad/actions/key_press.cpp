#include "stdafx.h"

#include "key_press.h"

KeyPress::KeyPress(ScopedKeyboard* keyboard, Keyboard::KeyCode key_code)
    : keyboard_(keyboard), key_code_(key_code) {}

KeyPress::~KeyPress() { assert(!key_press_token_); }

void KeyPress::OnActivated() {
  key_press_token_.emplace(keyboard_->GetKeyPressToken(key_code_));
}

void KeyPress::OnDeactivated() { key_press_token_.reset(); }
