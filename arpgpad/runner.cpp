#include "stdafx.h"

#include "actions/press_button_action.h"
#include "runner.h"

Runner::Runner()
    : keyboard_(&keyboard_and_mouse_), mouse_(&keyboard_and_mouse_) {}

Runner::~Runner() {}

bool Runner::LoadConfig(const Config& config) {
  std::vector<Controller*> controllers =
      controller_provider_xinput_.GetControllers();
  if (controllers.empty()) {
    LOG(ERR) << "No controllers";
    return false;
  }

  // Clear old state.
  for (const auto& action : actions_) {
    input_handler_->DeregisterAction(action.get());
  }
  actions_.clear();
  input_handler_.reset();

  config_ = config;

  using ButtonAction = Config::ButtonAction;

  auto button_action_to_callback = [this](const ButtonAction& button_action)
      -> std::function<ScopedDestructor()> {
    switch (button_action.type) {
      case ButtonAction::Type::kMouse: {
        Mouse::Button mouse_button = button_action.code.mouse_button;
        return [this, mouse_button] {
          return mouse_.GetMousePressToken(mouse_button);
        };
      }
      case ButtonAction::Type::kKeyboard: {
        Keyboard::KeyCode key_code = button_action.code.key_code;
        return
            [this, key_code] { return keyboard_.GetKeyPressToken(key_code); };
      }
    }

    assert(false);
    return {};
  };

  input_handler_.emplace(&display_, &mouse_, *config_,
                         button_action_to_callback(config_->move_command));
  controllers[0]->SetDelegate(&input_handler_.value());

  for (const auto& item : config_->button_to_action) {
    const ButtonAction& button_action = item.second;
    auto action = std::make_unique<PressButtonAction>(
        &input_handler_.value(), &keyboard_, *config_, button_action.no_move,
        button_action.distance_fraction,
        button_action_to_callback(button_action));

    input_handler_->RegisterAction(item.first, action.get());
    actions_.push_back(std::move(action));
  }

  LOG(INFO) << "Enabled config: " << config_->name;
  return true;
}

void Runner::Poll() {
  input_handler_->Poll();
  controller_provider_xinput_.PollControllers();
  for (const auto& action : actions_) {
    action->Poll();
  }
}
