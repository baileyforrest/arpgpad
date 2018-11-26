#include "stdafx.h"

#include "press_button_action.h"

PressButtonAction::PressButtonAction(InputHandler* input_handler,
                                     ScopedKeyboard* keyboard,
                                     const Config& config, bool no_move,
                                     float distance,
                                     PressButtonImpl press_button_impl)
    : input_handler_(input_handler),
      keyboard_(keyboard),
      no_move_(no_move),
      distance_(distance),
      press_delay_(config.mouse_position_delay_ms),
      press_button_impl_(std::move(press_button_impl)) {
  assert(distance >= 0.0f && distance <= 1.0f);
}

PressButtonAction::~PressButtonAction() {}

void PressButtonAction::OnActivated() {
  if (no_move_) {
    move_override_token_.emplace(input_handler_->OverrideMove());
    shift_key_token_.emplace(keyboard_->GetKeyPressToken(VK_LSHIFT));
  }
  if (distance_ != 0.0) {
    move_radius_override_token_.emplace(input_handler_->OverrideMoveRadius(
        distance_ * input_handler_->display()->GetHeight()));
    pending_press_time_ = std::chrono::steady_clock::now() + press_delay_;
  } else {
    press_token_.emplace(press_button_impl_());
  }
}

void PressButtonAction::OnDeactivated() {
  if (!pending_press_time_) {
    DoUnpress();
  }
}

void PressButtonAction::Poll() {
  if (pending_press_time_ &&
      std::chrono::steady_clock::now() > pending_press_time_) {
    pending_press_time_.reset();
    if (!press_token_) {
      press_token_.emplace(press_button_impl_());
    }
  }
}

void PressButtonAction::DoUnpress() {
  press_token_.reset();

  move_radius_override_token_.reset();

  shift_key_token_.reset();
  move_override_token_.reset();
}
