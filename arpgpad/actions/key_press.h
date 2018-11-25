#include <optional>

#include "action.h"
#include "scoped_keyboard.h"

class KeyPress : public Action {
 public:
  KeyPress(ScopedKeyboard* key_board, Keyboard::KeyCode key_code);
  ~KeyPress() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;

 private:
  ScopedKeyboard* const keyboard_;
  const Keyboard::KeyCode key_code_;

  std::optional<ScopedDestructor> key_press_token_;
};
