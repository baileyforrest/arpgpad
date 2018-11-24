#pragma once

#include <iostream>
#include <sstream>

// TODO: Add time
class Logger {
 public:
  enum class Type {
    INFO,
    ERR,
  };

  static const char* TypeString(Type type);

  Logger(Type type, const char* file, int line);
  ~Logger();

  std::ostream& stream() { return stream_; }

 private:
  std::ostringstream stream_;
};

#define LOG(type) Logger(Logger::Type::type, __FILE__, __LINE__).stream()
