#pragma once

#include <chrono>
#include <deque>
#include <functional>
#include <map>
#include <optional>

#include "action.h"
#include "config.h"
#include "controller.h"
#include "display.h"
#include "scoped_destructor.h"
#include "scoped_keyboard.h"
#include "scoped_mouse.h"
#include "vec2.h"

class InputHandler : public Controller::Delegate {
 public:
  using MoveCommand = std::function<ScopedDestructor()>;

  InputHandler(Display* display, ScopedMouse* mouse, const Config& config,
               MoveCommand move_command);
  ~InputHandler();

  void Poll();

  void RegisterAction(Controller::Button button, Action* action);
  void DeregisterAction(Action* action);

  ScopedDestructor OverrideMoveRadius(float radius);
  ScopedDestructor OverrideMove();

  Display* display() const { return display_; }

 private:
  using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

  // Controller::Delegate implementation:
  void OnStateChanged(const Controller::State& state) override;
  void HandleLStick(const Controller::State& state);
  void HandleRStick(const Controller::State& state);
  void RefreshMoveMousePosition();
  void RefreshCursorMousePosition(SteadyTimePoint now);

  Display* const display_;
  ScopedMouse* const mouse_;

  const float move_radius_;
  const FloatVec2 middle_;

  // The game may use a delayed or average mouse position. So when we want to
  // start to moving, we need to wait some time before clicking the button.
  const std::chrono::milliseconds start_move_delay_;

  const MoveCommand move_command_;

  // Last time |Poll| was called.
  SteadyTimePoint last_poll_time_;

  struct ActionState {
    Action* action;
    bool activated;
  };
  std::map<Controller::Button, ActionState> button_to_action_;

  bool is_moving_ = false;

  int move_override_count_ = 0;

  std::optional<ScopedDestructor> move_token_;
  std::optional<ScopedDestructor> cursor_mode_click_token_;

  // Used for delayed movement start. See |start_move_delay_|.
  std::optional<SteadyTimePoint> pending_start_move_time_;

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
