#pragma once

#include <chrono>
#include <functional>
#include <optional>

#include "config.h"
#include "input_handler.h"
#include "scoped_keyboard.h"

class PressButtonAction : public Action {
 public:
  using PressButtonImpl = std::function<ScopedDestructor()>;

  // |distance| is fraction of the vertical screen height
  PressButtonAction(InputHandler* input_handler, ScopedKeyboard* key_board,
                    const Config& config, bool no_move, float distance,
                    PressButtonImpl press_button_impl);
  ~PressButtonAction() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;
  void Poll() override;

 private:
  using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

  void DoUnpress();

  InputHandler* const input_handler_;
  ScopedKeyboard* const keyboard_;
  const bool no_move_;
  const float distance_;

  // The game may use a delayed or average mouse position. So when we want to
  // do a distance press, we need to wait some time before clicking the button.
  const std::chrono::milliseconds press_delay_;
  const PressButtonImpl press_button_impl_;

  std::optional<ScopedDestructor> press_token_;
  std::optional<ScopedDestructor> move_override_token_;
  std::optional<ScopedDestructor> move_radius_override_token_;
  std::optional<ScopedDestructor> shift_key_token_;

  // Used for delayed button press. See |start_move_delay_|.
  std::optional<SteadyTimePoint> pending_press_time_;
};
