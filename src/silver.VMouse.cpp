#include <atomic>          // For std::atomic
#include <mutex>           // For std::mutex and std::lock_guard
#include <thread>          // For std::thread
#include "silver.Keyboard.hpp"
#include "silver.VMouse.hpp"
#include "silver.hpp"

using namespace std;
atomic<bool> VMouse = false;
mutex mouseMutex;

void VMouseOn(int l, int r, int u, int d, int c) {
  while (VMouse.load()) {
    char det = silver.keyboard.getKey();

    {

      lock_guard<mutex> lock(mouseMutex);
      if (det == l) {
        silver.mouse.cursorPositionX--;
      } else if (det == r) {
        silver.mouse.cursorPositionX++;
      } else if (det == u) {
        silver.mouse.cursorPositionY--;
      } else if (det == d) {
        silver.mouse.cursorPositionY++;
      } else if (det == c) {
      }
    }
  }
}

void Silver::Mouse::startVMouse(int l, int r, int u, int d, int c) {
  silver.camera.hideMouse = false;
  if (VMouse.load())
    return;

  VMouse.store(true);
  mouseKey = c;
  thread vmouseThread(VMouseOn, l, r, u, d, c);
  vmouseThread.detach();
}

void Silver::Mouse::stopVMouse() { VMouse.store(false); }

bool Silver::Mouse::isMouse() {
  if (silver.keyboard.keyBuffer == mouseKey) {
    return true;
  }
  return false;
}
