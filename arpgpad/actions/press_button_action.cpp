#include "stdafx.h"

#include "press_button_action.h"

PressButtonAction::PressButtonAction(PressButtonImpl press_button_impl)
    : press_button_impl_(std::move(press_button_impl)) {}

PressButtonAction::~PressButtonAction() { assert(!press_token_); }

void PressButtonAction::OnActivated() {
  press_token_.emplace(press_button_impl_());
}

void PressButtonAction::OnDeactivated() { press_token_.reset(); }
