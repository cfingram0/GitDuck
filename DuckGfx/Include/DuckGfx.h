#pragma once
#include "windows.h"

namespace duckGfx {
  bool Init(HWND window);
  void Render();
  void ShutDown();
}