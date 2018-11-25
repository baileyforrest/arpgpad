#include <functional>
#include <optional>

#include "action.h"
#include "scoped_destructor.h"

class PressButtonAction : public Action {
 public:
  using PressButtonImpl = std::function<ScopedDestructor()>;

  PressButtonAction(PressButtonImpl press_button_impl);
  ~PressButtonAction() override;

  // Action implementation:
  void OnActivated() override;
  void OnDeactivated() override;

 protected:
  PressButtonImpl press_button_impl_;
  std::optional<ScopedDestructor> press_token_;
};
