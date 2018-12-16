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
    {"CTRL", VK_CONTROL, },
    {"TAB", VK_TAB, },
    {"ENTER", VK_RETURN, },
    {"ESC", VK_ESCAPE, },
    {"SPACE", VK_SPACE, },
    {"F1", VK_F1, },
    {"F1", VK_F1, },
    {"F2", VK_F2, },
    {"F3", VK_F3, },
    {"F4", VK_F4, },
    {"F5", VK_F5, },
    {"F6", VK_F6, },
    {"F7", VK_F7, },
    {"F8", VK_F8, },
    {"F9", VK_F9, },
    {"F10", VK_F10, },
    {"F11", VK_F11, },
    {"F12", VK_F12, },
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
