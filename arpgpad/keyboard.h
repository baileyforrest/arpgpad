#pragma once

#include <cstdint>
#include <string>

class Keyboard {
 public:
  using KeyCode = int32_t;
  static std::string KeyCodeToString(KeyCode code);

  virtual void NotifyKeyPress(KeyCode code) = 0;
  virtual void NotifyKeyRelease(KeyCode code) = 0;

 protected:
  virtual ~Keyboard() = default;
};
