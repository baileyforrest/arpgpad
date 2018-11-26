#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "input_handler.h"
#include "log.h"

namespace {

// TODO: Possibly make configurable.
constexpr float kLStickThreshold = 0.15f;
constexpr float kRStickThreshold = 0.15f;
constexpr float kTriggerThreshold = 0.1f;
constexpr float kCursorMoveSpeedSlowMult = 1.0f;
constexpr float kCursorMoveSpeedFastMult = 2.0f;

}  // namespace

InputHandler::InputHandler(Display* display, ScopedMouse* mouse,
                           const Config& config, MoveCommand move_command)
    : mouse_(mouse),
      display_(display),
      move_radius_(config.move_radius_fraction * display_->GetHeight()),
      middle_(display_->GetWidth() / 2.0f,
              display_->GetHeight() * config.middle_offset_fraction),
      start_move_delay_(config.mouse_position_delay_ms),
      move_command_(std::move(move_command)) {
  assert(config.move_radius_fraction > 0.0f &&
         config.move_radius_fraction < 1.0f);
  assert(config.middle_offset_fraction > 0.0f &&
         config.middle_offset_fraction < 1.0f);
}
InputHandler::~InputHandler() = default;

void InputHandler::Poll() {
  SteadyTimePoint now = std::chrono::steady_clock::now();

  if (pending_start_move_time_ && now > pending_start_move_time_) {
    pending_start_move_time_.reset();
    if (is_moving_ && move_override_count_ == 0 && !move_token_) {
      move_token_.emplace(move_command_());
    }
  }

  RefreshCursorMousePosition(now);
  last_poll_time_ = now;
}

void InputHandler::RegisterAction(Controller::Button button, Action* action) {
  auto ret = button_to_action_.emplace(button, ActionState{action, false});
  assert(ret.second);
}

void InputHandler::DeregisterAction(Action* action) {
  for (auto it = button_to_action_.begin(); it != button_to_action_.end();) {
    if (it->second.action == action) {
      it = button_to_action_.erase(it);
    } else {
      ++it;
    }
  }
}

ScopedDestructor InputHandler::OverrideMoveRadius(float radius) {
  uint64_t id = next_radius_override_id_++;
  id_to_mouse_radius_override_.push_back(MouseRadiusOverride{id, radius});
  RefreshMoveMousePosition();

  return ScopedDestructor([this, id] {
    id_to_mouse_radius_override_.erase(
        std::remove_if(id_to_mouse_radius_override_.begin(),
                       id_to_mouse_radius_override_.end(),
                       [id](const auto& radius_override) {
                         return radius_override.id == id;
                       }),
        id_to_mouse_radius_override_.end());
  });
}

ScopedDestructor InputHandler::OverrideMove() {
  if (++move_override_count_ == 1) {
    if (is_moving_) {
      move_token_.reset();
    }
  }

  return ScopedDestructor([this] {
    if (--move_override_count_ == 0) {
      if (is_moving_) {
        move_token_.emplace(move_command_());
      }
    }
  });
}

void InputHandler::OnStateChanged(const Controller::State& state) {
  HandleLStick(state);
  if (!is_moving_) {
    HandleRStick(state);
  }

  // Handle right trigger as 'left click'.
  if (state.rtrigger >= kTriggerThreshold) {
    if (!cursor_mode_click_token_) {
      cursor_mode_click_token_.emplace(
          mouse_->GetMousePressToken(Mouse::kButtonLeft));
    }
  } else {
    if (cursor_mode_click_token_) {
      cursor_mode_click_token_.reset();
    }
  }

  // Handle buttons and actions.
  for (int i = 1; i < Controller::kButtonMaxButton; i <<= 1) {
    auto button = static_cast<Controller::Button>(i);
    auto it = button_to_action_.find(button);
    if (it == button_to_action_.end()) {
      continue;
    }

    bool pressed = state.buttons & button;
    if (pressed == it->second.activated) {
      continue;
    }
    it->second.activated = pressed;

    if (pressed) {
      it->second.action->OnActivated();
    } else {
      it->second.action->OnDeactivated();
    }
  }
}

void InputHandler::HandleLStick(const Controller::State& state) {
  const FloatVec2& lstick = state.lstick;

  // Handle stop moving
  if (lstick.Magnitude() < kLStickThreshold) {
    if (is_moving_) {
      is_moving_ = false;
      move_token_.reset();
    }

    return;
  }

  is_moving_ = true;

  direction_ = lstick.Normal();

  // Swap direction because screen coordinates are reversed.
  direction_.y() *= -1.0f;
  RefreshMoveMousePosition();

  pending_start_move_time_ =
      std::chrono::steady_clock::now() + start_move_delay_;
}

void InputHandler::HandleRStick(const Controller::State& state) {
  const FloatVec2& rstick = state.rstick;
  float magnitude = rstick.Magnitude();
  if (magnitude < kRStickThreshold) {
    cursor_mouse_velocity_ = FloatVec2();
    return;
  }

  float velocity = (std::pow(16.0f, magnitude) - 1) / 16;

  // Left trigger controls cursor speed.
  float multiplier = state.ltrigger > kTriggerThreshold
                         ? kCursorMoveSpeedFastMult
                         : kCursorMoveSpeedSlowMult;
  velocity *= display_->GetHeight() * multiplier;

  cursor_mouse_velocity_ = rstick.Normal().Scale(velocity);

  // Swap direction because screen coordinates are reversed.
  cursor_mouse_velocity_.y() *= -1.0f;
}

void InputHandler::RefreshMoveMousePosition() {
  float radius = id_to_mouse_radius_override_.empty()
                     ? move_radius_
                     : id_to_mouse_radius_override_.back().radius;

  // Apply perspective. This is needed so the distance from the character in
  // game is normalized.
  // TODO: Pass this into the class. See if we can use one value.
  // TODO: Remove if this is no longer used.
  FloatVec2 perspective_direction = direction_.Scale(radius);
#if 0
  if (perspective_direction.y() < 0) {
    perspective_direction.y() *= 0.8f;
  } else {
    perspective_direction.y() *= 1.1f;
  }
#endif

  FloatVec2 target = middle_ + perspective_direction;

  mouse_->SetCursorPos(static_cast<int>(target.x()),
                       static_cast<int>(target.y()));
}

void InputHandler::RefreshCursorMousePosition(SteadyTimePoint now) {
  // Don't update cursor mode mouse when we're moving.
  if (is_moving_) {
    return;
  }

  if (cursor_mouse_velocity_ == FloatVec2(0, 0)) {
    return;
  }

  std::pair<int, int> mouse_pos_int = mouse_->GetCursorPos();
  // If mouse moved from our cached state, just reset the cache.
  if (static_cast<int>(cursor_mouse_position_.x()) != mouse_pos_int.first ||
      static_cast<int>(cursor_mouse_position_.y()) != mouse_pos_int.second) {
    cursor_mouse_position_ =
        FloatVec2(static_cast<float>(mouse_pos_int.first),
                  static_cast<float>(mouse_pos_int.second));
  }

  auto time_elapsed = now - last_poll_time_;
  float scale = static_cast<float>(
      std::chrono::duration<double>(time_elapsed) /
      std::chrono::duration<double>(std::chrono::seconds(1)));

  cursor_mouse_position_ += cursor_mouse_velocity_.Scale(scale);

  // Make sure position is in bounds.
  cursor_mouse_position_.x() =
      std::min(std::max(cursor_mouse_position_.x(), 0.0f),
               static_cast<float>(display_->GetWidth() - 1));
  cursor_mouse_position_.y() =
      std::min(std::max(cursor_mouse_position_.y(), 0.0f),
               static_cast<float>(display_->GetHeight() - 1));

  int new_x = static_cast<int>(cursor_mouse_position_.x());
  int new_y = static_cast<int>(cursor_mouse_position_.y());

  // Don't update position if it didn't actually change.
  if (mouse_pos_int.first == new_x && mouse_pos_int.second == new_y) {
    return;
  }

  mouse_->SetCursorPos(new_x, new_y);
}
