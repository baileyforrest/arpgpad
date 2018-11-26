#pragma once

#include <cstdint>
#include <iostream>
#include <optional>

#include "vec2.h"

class Controller {
 public:
  enum Button {
    kButtonA = 1 << 0,
    kButtonB = 1 << 1,
    kButtonX = 1 << 2,
    kButtonY = 1 << 3,
    kButtonLb = 1 << 4,
    kButtonRb = 1 << 5,
    kButtonBack = 1 << 6,
    kButtonStart = 1 << 7,
    kButtonLStick = 1 << 8,
    kButtonRStick = 1 << 9,
    kButtonDpadUp = 1 << 10,
    kButtonDpadDown = 1 << 11,
    kButtonDpadLeft = 1 << 12,
    kButtonDpadRight = 1 << 13,

    kButtonMaxButton = 1 << 14,
  };

  struct State {
    FloatVec2 lstick;
    FloatVec2 rstick;
    float ltrigger;
    float rtrigger;

    // Bit mask of buttons.
    uint32_t buttons = 0;
  };

  class Delegate {
   public:
    // The implementation of Controller must correctly implement deadzone
    // detection.
    virtual void OnStateChanged(const State& state) = 0;

   protected:
    ~Delegate() = default;
  };

  static const char* ButtonToString(Button button);
  static std::optional<Button> StringToButton(const std::string& string);

  virtual void SetDelegate(Delegate* d) = 0;

 protected:
  virtual ~Controller() = default;
};

std::ostream& operator<<(std::ostream& stream, const Controller::State& state);
bool operator==(const Controller::State& lhs, const Controller::State& rhs);
bool operator!=(const Controller::State& lhs, const Controller::State& rhs);