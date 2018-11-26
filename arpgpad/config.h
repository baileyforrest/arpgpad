#pragma once

#include <map>
#include <memory>
#include <string>

#include "controller.h"
#include "keyboard.h"
#include "mouse.h"

struct Config {
  std::string name;

  // Fraction of vertical screen resolution to use for movement distance.
  float move_radius_fraction = 0.0;

  // Fraction of vertical screen resolution for the tocation of character on
  // the screen. 0: top of screen, 1: Buttom of screen.
  float middle_offset_fraction = 0.0;

  // Time to wait before performing actions based on mouse position.
  int mouse_position_delay_ms = 0;

  struct ButtonAction {
    static ButtonAction KeyAction(Keyboard::KeyCode key_code,
                                  bool no_move = false, float distance = 0.0f);
    static ButtonAction MouseAction(Mouse::Button button, bool no_move = false,
                                    float distance = 0.0f);

    enum class Type {
      kMouse,
      kKeyboard,
    };
    Type type = Type::kMouse;

    union {
      Mouse::Button mouse_button;
      Keyboard::KeyCode key_code;
    } code{};

    bool no_move = false;

    // Fraction of screen press should be away from the center. 0.0 means use
    // |move_radius_fraction|.
    float distance_fraction = 0.0f;
  };

  ButtonAction move_command;
  std::map<Controller::Button, ButtonAction> button_to_action;
};
