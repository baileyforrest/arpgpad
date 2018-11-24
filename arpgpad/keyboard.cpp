#include "stdafx.h"

#include "keyboard.h"

// static
std::string Keyboard::KeyCodeToString(KeyCode code) {
  char name[1024];
  UINT scan_code = MapVirtualKeyA(code, MAPVK_VK_TO_VSC);
  if (GetKeyNameTextA(scan_code << 16, name, sizeof(name)) <= 0) {
    return "Invalid key code: " + std::to_string(GetLastError());
  }

  return name;
}
