#pragma once

#include <functional>
#include <optional>

#include "input_handler.h"
#include "scoped_keyboard.h"

class PressButtonAction : public Action {
 public:
  using PressButtonImpl = std::function<ScopedDestructor()>;

  // |distance| is fraction of the vertical screen height
  PressButtonAction(InputHandler* input_handler, ScopedKeyboard* key_board,
                    PressButtonImpl press_button_impl, bool no_move = false,
                    float distance = 0.0);
  ~PressButtonAction() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;

 private:
  InputHandler* const input_handler_;
  ScopedKeyboard* const keyboard_;
  const PressButtonImpl press_button_impl_;
  const bool no_move_;
  const float distance_;

  std::optional<ScopedDestructor> press_token_;
  std::optional<ScopedDestructor> move_override_token_;
  std::optional<ScopedDestructor> move_radius_override_token_;
  std::optional<ScopedDestructor> shift_key_token_;
};
