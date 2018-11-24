#include "stdafx.h"

#include <cassert>
#include <ctime>
#include <iomanip>

#include "log.h"

// static
const char* Logger::TypeString(Type type) {
  switch (type) {
    case Type::INFO:
      return "INFO";
    case Type::ERR:
      return "ERR";
  }
  assert(false);
  return "";
}

Logger::Logger(Type type, const char* file, int line) {
  std::string filename = file;
  auto pos = filename.find_last_of("\\");
  if (pos != std::string::npos) {
    filename = filename.substr(pos + 1);
  }

  std::time_t now = std::time(nullptr);
  struct tm now_tm;
  localtime_s(&now_tm, &now);

  stream_ << "[" << std::put_time(&now_tm, "%m%e/%T") << ":" << TypeString(type)
          << ":" << filename << "(" << line << ")] ";
}

Logger::~Logger() {
  stream_ << "\n";
  std::string str = stream_.str();
  std::wstring wide_str(str.begin(), str.end());
  OutputDebugString(wide_str.c_str());
}