#pragma once

#include "controller.h"

class LoggingControllerDelegate : public Controller::Delegate {
 public:
  // Controller::Delegate implementation:
  void OnStateChanged(const Controller::State& state) override;
};
