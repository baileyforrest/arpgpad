#include "stdafx.h"

#include "key_press_no_move.h"

KeyPressNoMove::KeyPressNoMove(InputHandler* input_handler,
                               ScopedKeyboard* keyboard,
                               Keyboard::KeyCode key_code)
    : KeyPress(keyboard, key_code), input_handler_(input_handler) {}

KeyPressNoMove::~KeyPressNoMove() {}

void KeyPressNoMove::OnActivated() {
  move_override_token_.emplace(input_handler_->OverrideMove());
  shift_key_token_.emplace(keyboard_->GetKeyPressToken(VK_LSHIFT));
  KeyPress::OnActivated();
}

void KeyPressNoMove::OnDeactivated() {
  KeyPress::OnDeactivated();
  shift_key_token_.reset();
  move_override_token_.reset();
}
