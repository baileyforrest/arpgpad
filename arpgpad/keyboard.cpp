#include "stdafx.h"

#include "keyboard.h"

#include <cctype>

namespace {

// clang-format off
struct {
  const char* string;
  Keyboard::KeyCode code;
} kStringToKeyCode[] = {
    {"BACK", VK_BACK, },
    {"TAB", VK_TAB, },
    {"ENTER", VK_RETURN, },
    {"ESC", VK_ESCAPE, },
    {"SPACE", VK_SPACE, },
};
// clang-format on

}  // namespace

// static
std::string Keyboard::KeyCodeToString(KeyCode code) {
  char name[1024];
  UINT scan_code = MapVirtualKeyA(code, MAPVK_VK_TO_VSC);
  if (GetKeyNameTextA(scan_code << 16, name, sizeof(name)) <= 0) {
    return "Invalid key code: " + std::to_string(GetLastError());
  }

  return name;
}

std::optional<Keyboard::KeyCode> Keyboard::ParseKeyCode(
    const std::string& string) {
  for (const auto& item : kStringToKeyCode) {
    if (item.string == string) {
      return item.code;
    }
  }

  if (string.size() == 1 && std::isalnum(string[0])) {
    return std::toupper(string[0]);
  }

  return std::nullopt;
}
