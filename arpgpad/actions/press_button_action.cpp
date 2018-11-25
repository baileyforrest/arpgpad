#include "stdafx.h"

#include "press_button_action.h"

PressButtonAction::PressButtonAction(InputHandler* input_handler,
                                     ScopedKeyboard* keyboard, bool no_move,
                                     float distance,
                                     PressButtonImpl press_button_impl)
    : input_handler_(input_handler),
      keyboard_(keyboard),
      press_button_impl_(std::move(press_button_impl)),
      no_move_(no_move),
      distance_(distance) {
  assert(distance >= 0.0f && distance <= 1.0f);
}

PressButtonAction::~PressButtonAction() {
  assert(!press_token_);
  assert(!move_override_token_);
  assert(!shift_key_token_);
}

void PressButtonAction::OnActivated() {
  if (no_move_) {
    move_override_token_.emplace(input_handler_->OverrideMove());
    shift_key_token_.emplace(keyboard_->GetKeyPressToken(VK_LSHIFT));
  }
  if (distance_ != 0.0) {
    move_radius_override_token_.emplace(
        input_handler_->OverrideMoveRadius(distance_));
  }
  press_token_.emplace(press_button_impl_());
}

void PressButtonAction::OnDeactivated() {
  press_token_.reset();

  move_radius_override_token_.reset();

  shift_key_token_.reset();
  move_override_token_.reset();
}
