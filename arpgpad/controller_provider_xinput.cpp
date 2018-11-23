#include "stdafx.h"

#include "controller_provider_xinput.h"

#include "Xinput.h"

#include <cassert>

#include "log.h"

namespace {

Controller::State XInputStateToState(const XINPUT_STATE& xinput) {
  const XINPUT_GAMEPAD gamepad = xinput.Gamepad;

  Controller::State state;
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
    state.buttons |= Controller::kButtonDpadUp;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
    state.buttons |= Controller::kButtonDpadDown;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
    state.buttons |= Controller::kButtonDpadLeft;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
    state.buttons |= Controller::kButtonDpadRight;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_START) {
    state.buttons |= Controller::kButtonStart;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
    state.buttons |= Controller::kButtonBack;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) {
    state.buttons |= Controller::kButtonLStick;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
    state.buttons |= Controller::kButtonRStick;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
    state.buttons |= Controller::kButtonLb;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
    state.buttons |= Controller::kButtonRb;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_A) {
    state.buttons |= Controller::kButtonA;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_B) {
    state.buttons |= Controller::kButtonB;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_X) {
    state.buttons |= Controller::kButtonX;
  }
  if (gamepad.wButtons & XINPUT_GAMEPAD_Y) {
    state.buttons |= Controller::kButtonY;
  }

  state.ltrigger = gamepad.bLeftTrigger / 255.0f;
  state.rtrigger = gamepad.bRightTrigger / 255.0f;

  static constexpr float kDivisor = 32768.0f;
  state.lstick.set_x(gamepad.sThumbLX / kDivisor);
  state.lstick.set_y(gamepad.sThumbLY / kDivisor);
  state.rstick.set_x(gamepad.sThumbRX / kDivisor);
  state.rstick.set_y(gamepad.sThumbRY / kDivisor);

  return state;
}

}  // namespace

class ControllerProviderXInput::ControllerImpl : public Controller {
 public:
  ControllerImpl() = default;
  ~ControllerImpl() override = default;

  // Controller implementation:
  void SetDelegate(Delegate* delegate) override { delegate_ = delegate; }

  void NotifyStateChanged(const State& state, DWORD packet_number) {
    state_ = state;
    packet_number_ = packet_number;
    if (delegate_) {
      delegate_->OnStateChanged(state_);
    }
  }

  const State& state() const { return state_; }
  DWORD packet_number() const { return packet_number_; }

 private:
  Delegate* delegate_ = nullptr;
  State state_;
  DWORD packet_number_ = -1;
};

ControllerProviderXInput::ControllerProviderXInput() { LoadControllers(); }
ControllerProviderXInput::~ControllerProviderXInput() = default;

std::vector<Controller*> ControllerProviderXInput::GetControllers() {
  std::vector<Controller*> result;
  for (const auto& item : id_to_controllers_) {
    result.push_back(item.second.get());
  }

  return result;
}

void ControllerProviderXInput::PollControllers() {
  for (const auto& item : id_to_controllers_) {
    UpdateControllerState(item.first);
  }
}

void ControllerProviderXInput::LoadControllers() {
  for (DWORD id = 0; id < XUSER_MAX_COUNT; ++id) {
    UpdateControllerState(id);
  }
}

void ControllerProviderXInput::UpdateControllerState(DWORD id) {
  auto it = id_to_controllers_.find(id);

  XINPUT_STATE state{};
  DWORD result = XInputGetState(id, &state);
  if (result != ERROR_SUCCESS) {
    if (it != id_to_controllers_.end()) {
      id_to_controllers_.erase(it);
    }
    // TODO: Send callback to clients if we care about it.
    return;
  }

  if (it == id_to_controllers_.end()) {
    LOG(INFO) << "ControllerProviderXInput: Adding controller " << id;
    auto ret =
        id_to_controllers_.emplace(id, std::make_unique<ControllerImpl>());
    assert(ret.second);
    it = ret.first;
    // TODO: Send callback to clients if we care about it.
  }

  auto& controller = *it->second;
  if (state.dwPacketNumber != controller.packet_number()) {
    controller.NotifyStateChanged(XInputStateToState(state),
                                  state.dwPacketNumber);
  }
}
