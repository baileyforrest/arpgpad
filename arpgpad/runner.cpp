#include "stdafx.h"

#include "actions/press_button_action.h"
#include "runner.h"

namespace {

// TODO: Don't hard code these
Config PathOfExileConfig() {
  Config config;
  config.move_radius_fraction = 0.15f;
  config.middle_offset_fraction = 0.44f;
  config.mouse_position_delay_ms = 10;

  using BA = Config::ButtonAction;

  config.button_to_action.emplace(Controller::kButtonBack,
                                  BA::KeyAction(VK_ESCAPE));

  // Flasks
  config.button_to_action.emplace(Controller::kButtonDpadUp,
                                  BA::KeyAction('1'));
  config.button_to_action.emplace(Controller::kButtonDpadRight,
                                  BA::KeyAction('2'));
  config.button_to_action.emplace(Controller::kButtonDpadDown,
                                  BA::KeyAction('3'));
  config.button_to_action.emplace(Controller::kButtonDpadLeft,
                                  BA::KeyAction('4'));
  config.button_to_action.emplace(Controller::kButtonLStick,
                                  BA::KeyAction('5'));

  // Mouse
  config.button_to_action.emplace(Controller::kButtonA,
                                  BA::MouseAction(Mouse::kButtonRight));
  config.button_to_action.emplace(Controller::kButtonX,
                                  BA::MouseAction(Mouse::kButtonMiddle, true));

  // QWERT
  config.button_to_action.emplace(Controller::kButtonStart, BA::KeyAction('Q'));
  config.button_to_action.emplace(Controller::kButtonLb, BA::KeyAction('W'));
  config.button_to_action.emplace(Controller::kButtonRb, BA::KeyAction('E'));

  // I have RT mapped to mouse forward/back buttons.
  config.button_to_action.emplace(
      Controller::kButtonY, BA::MouseAction(Mouse::kButtonX2));  // Forward
  config.button_to_action.emplace(
      Controller::kButtonB,
      BA::MouseAction(Mouse::kButtonX1, true, 0.48f));  // Back

  return config;
}

}  // namespace

Runner::Runner()
    : keyboard_(&keyboard_and_mouse_), mouse_(&keyboard_and_mouse_) {}

Runner::~Runner() {}

bool Runner::Init() {
  std::vector<Controller*> controllers =
      controller_provider_xinput_.GetControllers();
  if (controllers.empty()) {
    LOG(ERR) << "No controllers";
    return false;
  }

  config_ = PathOfExileConfig();

  input_handler_.emplace(&display_, &mouse_, *config_);
  controllers[0]->SetDelegate(&input_handler_.value());

  for (const auto& item : config_->button_to_action) {
    using ButtonAction = Config::ButtonAction;

    const ButtonAction& button_action = item.second;

    std::unique_ptr<Action> action;
    switch (button_action.type) {
      case ButtonAction::Type::kMouse: {
        Mouse::Button mouse_button = button_action.code.mouse_button;
        action = std::make_unique<PressButtonAction>(
            &input_handler_.value(), &keyboard_, *config_,
            button_action.no_move, button_action.distance_fraction,
            [this, mouse_button] {
              return mouse_.GetMousePressToken(mouse_button);
            });
        break;
      }
      case ButtonAction::Type::kKeyboard: {
        Keyboard::KeyCode key_code = button_action.code.key_code;
        action = std::make_unique<PressButtonAction>(
            &input_handler_.value(), &keyboard_, *config_,
            button_action.no_move, button_action.distance_fraction,
            [this, key_code] { return keyboard_.GetKeyPressToken(key_code); });
        break;
      }
    }

    assert(action);

    Controller::Button button = item.first;
    input_handler_->RegisterAction(button, action.get());

    actions_.push_back(std::move(action));
  }

  return true;
}

void Runner::Poll() {
  input_handler_->Poll();
  controller_provider_xinput_.PollControllers();
  for (const auto& action : actions_) {
    action->Poll();
  }
}
