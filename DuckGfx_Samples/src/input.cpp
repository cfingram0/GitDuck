#include "input.h"
#include <windows.h>
#include <cstring>
namespace input {
  bool gKeysCurrent[Key::count];
  bool gKeysPrev[Key::count];

  bool IsPressed(Key k) {
    return gKeysCurrent[k];
  }
  
  bool IsTriggered(Key k) {
    return gKeysCurrent[k] && !gKeysPrev[k];
  }

  void Update() {
    memcpy(gKeysPrev, gKeysCurrent, sizeof(bool) * Key::count);
  }

  static void setVal(uint64_t VK_code, bool val) {
    // if a letter
    if (VK_code >= 'A' && VK_code <= 'Z') {
      // in our own key mappeing, A is 0
      gKeysCurrent[VK_code - 'A'] = val;
      return;
    }

    // if a number
    if (VK_code >= '0' && VK_code <= '9') {
      gKeysCurrent[VK_code - '0' + Key::NUM_0] = val;
      return;
    }

    // special case
    if (VK_code == VK_SPACE) {
      gKeysCurrent[Key::SPACE] = val;
      return;
    }

    if (VK_code == VK_ESCAPE) {
      gKeysCurrent[Key::ESCAPE] = val;
    }
  }

  void HandleKeyDownMsg(uint64_t VK_code) {
    setVal(VK_code, true);
  }

  void HandleKeyUpMsg(uint64_t VK_code) {
    setVal(VK_code, false);
  }
}