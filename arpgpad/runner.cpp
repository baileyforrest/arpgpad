#include "stdafx.h"

#include "runner.h"

namespace {

// TODO: don't hard code.
InputHandler::Params CreateInputHandlerParams() {
  InputHandler::Params params;
  params.move_radius = 1440 * .15f;
  params.middle_offset = 0.44f;
  return params;
}

}  // namespace

Runner::Runner()
    : keyboard_(&keyboard_and_mouse_),
      mouse_(&keyboard_and_mouse_),
      input_handler_(&mouse_, CreateInputHandlerParams()) {}

Runner::~Runner() {}

bool Runner::Init() {
  std::vector<Controller*> controllers =
      controller_provider_xinput_.GetControllers();
  if (controllers.empty()) {
    LOG(ERR) << "No controllers";
    return false;
  }

  controllers[0]->SetDelegate(&input_handler_);
  return true;
}

void Runner::Poll() {
  input_handler_.Poll();
  controller_provider_xinput_.PollControllers();
}
