#include "stdafx.h"

#include <cassert>

#include "input_handler.h"

namespace {

// TODO: tune these values
constexpr float kLStickThreshold = 0.25f;

}  // namespace

InputHandler::InputHandler(Keyboard* keyboard, Mouse* mouse)
    : keyboard_(keyboard), mouse_(mouse) {}
InputHandler::~InputHandler() = default;

void InputHandler::RegisterAction(Controller::Button button, Action* action) {
  auto ret = button_to_action_.emplace(button, action);
  assert(ret.second);
}

void InputHandler::OnStateChanged(const Controller::State& state) {}

void InputHandler::HandleLStick(const Controller::State& state) {
  const FloatVec2& lstick = state.lstick;
  if (lstick.Magnitude() < kLStickThreshold) {
    if (is_moving_) {
      if (move_override_count_ == 0) {
      }
    }
  }
}
