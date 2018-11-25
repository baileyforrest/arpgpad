#include "stdafx.h"

#include "display.h"

Display::Display() {}

Display::~Display() {}

int Display::GetWidth() { return GetSystemMetrics(SM_CXSCREEN); }

int Display::GetHeight() { return GetSystemMetrics(SM_CYSCREEN); }
