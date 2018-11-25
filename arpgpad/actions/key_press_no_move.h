#include <optional>

#include "actions/key_press.h"
#include "input_handler.h"

class KeyPressNoMove : public KeyPress {
 public:
  KeyPressNoMove(InputHandler* input_handler, ScopedKeyboard* key_board,
                 Keyboard::KeyCode key_code);
  ~KeyPressNoMove() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;

 private:
  InputHandler* const input_handler_;
  std::optional<ScopedDestructor> move_override_token_;
  std::optional<ScopedDestructor> shift_key_token_;
};
