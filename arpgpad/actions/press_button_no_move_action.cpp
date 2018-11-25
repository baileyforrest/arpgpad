#include "stdafx.h"

#include "press_button_no_move_action.h"

PressButtonNoMoveAction::PressButtonNoMoveAction(
    InputHandler* input_handler, ScopedKeyboard* keyboard,
    PressButtonImpl press_button_impl)
    : PressButtonAction(std::move(press_button_impl)),
      input_handler_(input_handler),
      keyboard_(keyboard) {}

PressButtonNoMoveAction::~PressButtonNoMoveAction() {}

void PressButtonNoMoveAction::OnActivated() {
  move_override_token_.emplace(input_handler_->OverrideMove());
  shift_key_token_.emplace(keyboard_->GetKeyPressToken(VK_LSHIFT));
  PressButtonAction::OnActivated();
}

void PressButtonNoMoveAction::OnDeactivated() {
  PressButtonAction::OnDeactivated();
  shift_key_token_.reset();
  move_override_token_.reset();
}
