#pragma once

#include <vector>

#include "controller.h"

class ControllerProvider {
 public:
  // Owned by |this|.
  virtual std::vector<Controller*> GetControllers() = 0;

  virtual void PollControllers() {}

 protected:
  virtual ~ControllerProvider() = default;
};
