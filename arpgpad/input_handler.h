#pragma once

#include <chrono>
#include <deque>
#include <map>
#include <optional>

#include "action.h"
#include "controller.h"
#include "scoped_keyboard.h"
#include "scoped_mouse.h"
#include "vec2.h"

class InputHandler : public Controller::Delegate {
 public:
  InputHandler(Keyboard* keyboard, Mouse* mouse, float mouse_radius,
               float middle_offset);
  ~InputHandler();

  void Poll();

  void RegisterAction(Controller::Button button, Action* action);
  ScopedDestructor OverrideRadius(float radius);
  ScopedDestructor OverrideMove();

 private:
  using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

  // Controller::Delegate implementation:
  void OnStateChanged(const Controller::State& state) override;
  void HandleLStick(const Controller::State& state);
  void HandleRStick(const Controller::State& state);
  void RefreshMoveMousePosition();
  void RefreshCursorMousePosition(SteadyTimePoint now);

  // TODO: Don't hard code this.
  const int screen_width_ = 2560;
  const int screen_height_ = 1440;
  const float move_radius_;
  const FloatVec2 middle_;

  ScopedKeyboard keyboard_;
  ScopedMouse mouse_;

  // Last time |Poll| was called.
  SteadyTimePoint last_poll_time_;

  struct ActionState {
    Action* action;
    bool activated;
  };
  std::map<Controller::Button, ActionState> button_to_action_;

  bool is_moving_ = false;
  int move_override_count_ = 0;

  std::optional<ScopedDestructor> left_mouse_click_token_;

  // Direction character is facing.
  FloatVec2 direction_;

  // Mouse velocity for cursor mode.
  FloatVec2 cursor_mouse_velocity_;

  // Position of mouse in cursor mode. We use a FloatVec2 here to allow us to
  // keep track of fractional changes.
  FloatVec2 cursor_mouse_position_;

  // Next ID for entries in |id_to_radius_override_|.
  uint64_t next_radius_override_id_ = 0;

  // Queue to override current mouse radius. Most recent entry (end) wins.
  struct MouseRadiusOverride {
    uint64_t id;
    float radius;
  };
  std::deque<MouseRadiusOverride> id_to_mouse_radius_override_;
};
