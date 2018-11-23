#include "stdafx.h"

#include "logging_controller_delegate.h"

#include "log.h"

void LoggingControllerDelegate::OnStateChanged(const Controller::State& state) {
  LOG(INFO) << state;
}
