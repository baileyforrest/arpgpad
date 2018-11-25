#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "input_handler.h"

namespace {

// TODO: tune these values
constexpr float kLStickThreshold = 0.25f;
constexpr float kRStickThreshold = 0.16f;
constexpr float kTriggerThreshold = 0.10f;

}  // namespace

InputHandler::InputHandler(ScopedMouse* mouse, const Params& params)
    : move_radius_(params.move_radius),
      middle_(screen_width_ / 2.0f, screen_height_ * params.middle_offset),
      mouse_(mouse) {
  assert(params.move_radius > 0.0f);
  assert(params.middle_offset > 0.0f && params.middle_offset < 1.0f);
}
InputHandler::~InputHandler() = default;

void InputHandler::Poll() {
  SteadyTimePoint now = std::chrono::steady_clock::now();
  RefreshCursorMousePosition(now);
  last_poll_time_ = now;
}

void InputHandler::RegisterAction(Controller::Button button, Action* action) {
  auto ret = button_to_action_.emplace(button, ActionState{action, false});
  assert(ret.second);
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
      left_mouse_click_token_.reset();
    }
  }

  return ScopedDestructor([this] {
    if (--move_override_count_ == 0) {
      if (is_moving_) {
        left_mouse_click_token_.emplace(
            mouse_->GetMousePressToken(Mouse::kButtonLeft));
      }
    }
  });
}

void InputHandler::OnStateChanged(const Controller::State& state) {
  HandleLStick(state);
  if (!is_moving_) {
    HandleRStick(state);

    // Handle right trigger as 'left click'.
    if (state.rtrigger >= kTriggerThreshold) {
      if (!left_mouse_click_token_) {
        left_mouse_click_token_.emplace(
            mouse_->GetMousePressToken(Mouse::kButtonLeft));
      }
    } else {
      if (left_mouse_click_token_) {
        left_mouse_click_token_.reset();
      }
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
      left_mouse_click_token_.reset();
    }

    return;
  }

  is_moving_ = true;

  direction_ = lstick.Normal();

  // Swap direction because screen coordinates are reversed.
  direction_.y() *= -1.0f;
  RefreshMoveMousePosition();
  if (move_override_count_ == 0 && !left_mouse_click_token_) {
    left_mouse_click_token_.emplace(
        mouse_->GetMousePressToken(Mouse::kButtonLeft));
  }
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
  if (state.ltrigger > kTriggerThreshold) {
    // TODO: Should be configurable.
    velocity *= screen_height_ * 2.0f;
  } else {
    // TODO: Should be configurable.
    velocity *= screen_height_ * 1.0f;
  }

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
  FloatVec2 perspective_direction = direction_.Scale(radius);
  if (perspective_direction.y() < 0) {
    perspective_direction.y() *= 0.8f;
  } else {
    perspective_direction.y() *= 1.1f;
  }

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
               static_cast<float>(screen_width_ - 1));
  cursor_mouse_position_.y() =
      std::min(std::max(cursor_mouse_position_.y(), 0.0f),
               static_cast<float>(screen_height_ - 1));

  int new_x = static_cast<int>(cursor_mouse_position_.x());
  int new_y = static_cast<int>(cursor_mouse_position_.y());

  // Don't update position if it didn't actually change.
  if (mouse_pos_int.first == new_x && mouse_pos_int.second == new_y) {
    return;
  }

  mouse_->SetCursorPos(new_x, new_y);
}
