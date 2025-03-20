#include "Silver.hpp"
#include "SilverVMouse.hpp"

#include <atomic>
#include <mutex>
#include <thread>

using namespace std;

atomic<bool> VMouse = false;
mutex mouseMutex;

int mouseKey;
int cursorPositionX = 0;
int cursorPositionY = 0;
std::string mouseIcon = "O";
bool hideMouse = false;

void VMouseOn(int leftKey, int rightKey, int upKey, int downKey, int clickKey) {
  while (VMouse.load()) {
    lock_guard<mutex> lock(mouseMutex);
    if (IsKey(leftKey)) {
      cursorPositionX--;
    } else if (IsKey(rightKey)) {
      cursorPositionX++;
    } else if (IsKey(upKey)) {
      cursorPositionY--;
    } else if (IsKey(downKey)) {
      cursorPositionY++;
    }
  }
}

void StartVMouse(int leftKey, int rightKey, int upKey, int downKey, int clickKey) {
  hideMouse = false;
  if (VMouse.load())
    return;

  VMouse.store(true);
  mouseKey = clickKey;
  thread vmouseThread(VMouseOn, leftKey, rightKey, upKey, downKey, clickKey);
  vmouseThread.detach();
}

void StopVMouse() {
  hideMouse = true;
  VMouse.store(false);
}

bool WasMouseClicked() {
  if (IsKey(mouseKey)) {
    return true;
  }
  return false;
}
