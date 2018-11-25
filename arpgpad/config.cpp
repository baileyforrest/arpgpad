#include "stdafx.h"

#include "config.h"

// static
Config::ButtonAction Config::ButtonAction::KeyAction(Keyboard::KeyCode key_code,
                                                     bool no_move,
                                                     float distance) {
  ButtonAction result;
  result.type = Type::kKeyboard;
  result.code.key_code = key_code;
  result.no_move = no_move;
  result.distance_fraction = distance;

  return result;
}

// static
Config::ButtonAction Config::ButtonAction::MouseAction(Mouse::Button button,
                                                       bool no_move,
                                                       float distance) {
  ButtonAction result;
  result.type = Type::kMouse;
  result.code.mouse_button = button;
  result.no_move = no_move;
  result.distance_fraction = distance;

  return result;
}
