#pragma once

#include <functional>

class ScopedDestructor {
 public:
  ScopedDestructor(std::function<void()> on_destroy)
      : on_destroy_(std::move(on_destroy)) {}
  ScopedDestructor(ScopedDestructor&&) = default;

  ~ScopedDestructor() { on_destroy_(); }

 private:
  const std::function<void()> on_destroy_;
};
