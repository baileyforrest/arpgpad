// arpgpad.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "arpgpad.h"

#include <cassert>
#include <chrono>
#include <thread>

#include "macros.h"
#include "runner.h"

namespace {

constexpr int kMaxLoadStringSize = 100;
constexpr int kPollIntervalMs = 1;

HINSTANCE g_instance;                      // current instance
WCHAR g_title[kMaxLoadStringSize];         // The title bar text
WCHAR g_window_class[kMaxLoadStringSize];  // the main window class name

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
      auto wmId = static_cast<WORD>(wparam);
      // Parse the menu selections:
      switch (wmId) {
        case IDM_ABOUT:
          DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window,
                    AboutMenuHandler);
          break;
        case IDM_EXIT:
          DestroyWindow(window);
          break;
        default:
          return DefWindowProc(window, message, wparam, lparam);
      }
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);

      static const char kMsg[] = "Hello, World!";
      TextOutA(hdc, 5, 5, kMsg, static_cast<int>(strlen(kMsg)));

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
#define TRY(expr)                                                    \
  do {                                                               \
    if (!(expr)) {                                                   \
      auto message = std::string("\"") + #expr +                     \
                     "\" failed: " + std::to_string(GetLastError()); \
      LOG(ERR) << message;                                           \
      MessageBoxA(nullptr, message.c_str(), "Error", MB_ICONERROR);  \
      return EXIT_FAILURE;                                           \
    }                                                                \
  } while (0)

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

  // Create window:
  {
    HWND window = CreateWindowW(g_window_class, g_title, WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr,
                                nullptr, instance, nullptr);
    TRY(window);
    HWND edit_box = CreateWindowExA(WS_EX_CLIENTEDGE, "Edit", "test",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20,
                                    140, 20, window, nullptr, nullptr, nullptr);

    ShowWindow(window, n_cmd_show);
    UpdateWindow(window);
  }

  Runner runner;
  if (!runner.Init()) {
    std::string message = "Failed to initialize: ";
    message += " No controllers found";
    // TODO: Propagate error reason.
    // TODO: Allow user to select controller and remember preference.
    MessageBoxA(nullptr, message.c_str(), "Error", MB_ICONERROR);
    return EXIT_FAILURE;
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

    runner.Poll();
    Sleep(kPollIntervalMs);
  }

  assert(false && "Not reached");
  return 0;

#undef TRY
}
