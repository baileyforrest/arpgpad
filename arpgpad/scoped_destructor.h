#pragma once

#include <cassert>
#include <functional>

class ScopedDestructor {
 public:
  ScopedDestructor(std::function<void()> on_destroy)
      : on_destroy_(std::move(on_destroy)) {
    assert(on_destroy_);
  }
  ScopedDestructor(ScopedDestructor&&) = default;
  ~ScopedDestructor() {
    if (on_destroy_) {
      on_destroy_();
    }
  }

 private:
  std::function<void()> on_destroy_;
};
