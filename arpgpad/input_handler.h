#pragma once

#include <map>

#include "action.h"
#include "controller.h"
#include "scoped_keyboard.h"
#include "scoped_mouse.h"

class InputHandler : public Controller::Delegate {
 public:
  InputHandler(Keyboard* keyboard, Mouse* mouse);
  ~InputHandler();

  void RegisterAction(Controller::Button button, Action* action);

 private:
  // Controller::Delegate implementation:
  void OnStateChanged(const Controller::State& state) override;
  void HandleLStick(const Controller::State& state);

  ScopedKeyboard keyboard_;
  ScopedMouse mouse_;

  std::map<Controller::Button, Action*> button_to_action_;

  bool is_moving_ = false;

  // Actions may disable movement. Number of actions which are disabling
  // movement right now.
  int move_override_count_ = 0;
};
