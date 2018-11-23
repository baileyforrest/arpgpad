#pragma once

#include <map>
#include <memory>

#include "controller_provider.h"

class ControllerProviderXInput : public ControllerProvider {
 public:
  ControllerProviderXInput();
  ~ControllerProviderXInput() override;

  std::vector<Controller*> GetControllers() override;

  void PollControllers() override;

 private:
  class ControllerImpl;

  void LoadControllers();
  void UpdateControllerState(DWORD id);

  std::map<DWORD, std::unique_ptr<ControllerImpl>> id_to_controllers_;
};
