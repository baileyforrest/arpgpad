// arpgpad.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "arpgpad.h"

#include <Shlobj.h>

#include <cassert>
#include <chrono>
#include <thread>

#include "config_reader.h"
#include "macros.h"
#include "runner.h"

namespace {

constexpr int kMaxLoadStringSize = 100;
constexpr int kPollIntervalMs = 1;

constexpr int kDefaultWidth = 600;
constexpr int kDefaultHeight = 400;
constexpr int kConfigSelectWidth = 200;

enum {
  kIdInvalid,
  kIdConfigSelect,
};

HINSTANCE g_instance;                      // current instance
WCHAR g_title[kMaxLoadStringSize];         // The title bar text
WCHAR g_window_class[kMaxLoadStringSize];  // the main window class name
HWND g_config_select;

// TODO: Move other globals to here.
class Arpgpad {
 public:
  Arpgpad(const std::filesystem::path& config_path)
      : config_reader_(config_path) {}

  ConfigReader config_reader_;
  Runner runner_;
};

std::optional<Arpgpad> g_arpgpad;

// Message handler for about box.
INT_PTR CALLBACK AboutMenuHandler(HWND dialog, UINT message, WPARAM wparam,
                                  LPARAM /* lparam */) {
  switch (message) {
    case WM_INITDIALOG:
      return static_cast<INT_PTR>(TRUE);

    case WM_COMMAND: {
      auto low_word = static_cast<WORD>(wparam);
      if (low_word == IDOK || low_word == IDCANCEL) {
        EndDialog(dialog, low_word);
        return static_cast<INT_PTR>(TRUE);
      }
      break;
    }
  }
  return static_cast<INT_PTR>(FALSE);
}

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wparam,
                         LPARAM lparam) {
  switch (message) {
    case WM_COMMAND: {
      // Parse the menu selections:
      switch (wparam) {
        case IDM_ABOUT:
          DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window,
                    AboutMenuHandler);
          break;
        case IDM_EXIT:
          DestroyWindow(window);
          break;
      }

      // Child notifications
      auto low_word = static_cast<WORD>(wparam);
      auto high_word = static_cast<WORD>(wparam >> 16);
      switch (high_word) {
        case CBN_SELCHANGE: {
          if (low_word == kIdConfigSelect) {
            char name[256] = {};
            GetDlgItemTextA(window, kIdConfigSelect, name, arraysize(name) - 1);
            const auto& name_to_config =
                g_arpgpad->config_reader_.name_to_config();
            auto it = name_to_config.find(name);
            assert(it != name_to_config.end());
            if (!g_arpgpad->runner_.LoadConfig(it->second)) {
              std::string message = "Failed to initialize: ";
              message += " No controllers found";
              // TODO: Propagate error reason.
              // TODO: Allow user to select controller and remember preference.
              MessageBoxA(nullptr, message.c_str(), "Error", MB_ICONERROR);
            }
          }
          break;
        }
      }
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);
      EndPaint(window, &ps);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(window, message, wparam, lparam);
  }
  return 0;
}

}  // namespace

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE /* hPrevInstance */,
                      LPWSTR /* lpCmdLine */, int n_cmd_show) {
#define TRY2(expr, msg)                                                        \
  do {                                                                         \
    if (!(expr)) {                                                             \
      auto message = std::string(msg) + ": " + std::to_string(GetLastError()); \
      LOG(ERR) << message;                                                     \
      MessageBoxA(nullptr, message.c_str(), "Error", MB_ICONERROR);            \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
  } while (0)

#define TRY(expr) TRY2(expr, "\"" #expr "\" failed")

  g_instance = instance;  // Store instance handle in our global variable

  // Initialize global strings
  TRY(LoadStringW(instance, IDS_APP_TITLE, g_title, arraysize(g_title)) > 0);
  TRY(LoadStringW(instance, IDC_ARPGPAD, g_window_class,
                  arraysize(g_window_class)) > 0);

  // Register class
  {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_ARPGPAD));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ARPGPAD);
    wcex.lpszClassName = g_window_class;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    TRY(RegisterClassExW(&wcex) != 0);
  }

  std::wstring my_docs_path;
  {
    wchar_t* tmp;
    TRY2(SHGetKnownFolderPath(FOLDERID_Documents, /*flags=*/0, nullptr, &tmp) ==
             S_OK,
         "Failed to get My Documents");
    my_docs_path = tmp;
    CoTaskMemFree(tmp);
  }
  std::filesystem::path config_path = my_docs_path;
  config_path /= "My Games";
  config_path /= "arpgpad";
  config_path /= "configs";

  g_arpgpad.emplace(config_path);

  // Create window:
  {
    HWND window = CreateWindowW(g_window_class, g_title, WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, kDefaultWidth, kDefaultHeight,
                                nullptr, nullptr, instance, nullptr);
    TRY(window);

    g_config_select =
        CreateWindowA("ComboBox", nullptr,
                      WS_BORDER | CBS_DISABLENOSCROLL | WS_CHILD |
                          CBS_DROPDOWNLIST | WS_VISIBLE,
                      3, 3, kConfigSelectWidth, kConfigSelectWidth, window,
                      reinterpret_cast<HMENU>(kIdConfigSelect), g_instance, 0);
    for (const auto& item : g_arpgpad->config_reader_.name_to_config()) {
      SendMessageA(g_config_select, CB_ADDSTRING, 0,
                   reinterpret_cast<LPARAM>(item.first.c_str()));
    }

    ShowWindow(window, n_cmd_show);
    UpdateWindow(window);
  }

  HACCEL accel_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_ARPGPAD));
  TRY(accel_table);

  // Main message loop
  while (true) {
    // Handle message if they exist.
    MSG msg;
    if (PeekMessageA(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
      if (!GetMessage(&msg, nullptr, 0, 0)) {
        return static_cast<int>(msg.wParam);
      }
      if (!TranslateAccelerator(msg.hwnd, accel_table, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    g_arpgpad->runner_.Poll();
    Sleep(kPollIntervalMs);
  }

  assert(false && "Not reached");
  return 0;

#undef TRY
#undef TRY2
}
