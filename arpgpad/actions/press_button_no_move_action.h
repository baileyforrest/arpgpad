#include <functional>
#include <optional>

#include "actions/press_button_action.h"
#include "input_handler.h"
#include "scoped_keyboard.h"

class PressButtonNoMoveAction : public PressButtonAction {
 public:
  PressButtonNoMoveAction(InputHandler* input_handler, ScopedKeyboard* key_board,
                 PressButtonImpl press_button_impl);
  ~PressButtonNoMoveAction() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;

 private:
  InputHandler* const input_handler_;
  ScopedKeyboard* const keyboard_;

  std::optional<ScopedDestructor> move_override_token_;
  std::optional<ScopedDestructor> shift_key_token_;
};
