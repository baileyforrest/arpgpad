#pragma once

#include <cassert>
#include <iostream>
#include <sstream>

// TODO: Add time
class Logger {
 public:
  enum class Type {
    INFO,
    ERR,
  };

  static const char* TypeString(Type type) {
    switch (type) {
      case Type::INFO:
        return "INFO";
      case Type::ERR:
        return "ERR";
    }
    assert(false);
    return "";
  }

  Logger(Type type, const char* file, int line) {
    stream_ << "[" << file << ":" << line << "] " << TypeString(type) << ": ";
  }
  ~Logger() {
    stream_ << "\n";
    std::string str = stream_.str();
    std::wstring wide_str(str.begin(), str.end());
    OutputDebugString(wide_str.c_str());
  }

  std::ostream& stream() { return stream_; }

 private:
  std::ostringstream stream_;
};

#define LOG(type) Logger(Logger::Type::type, __FILE__, __LINE__).stream()
