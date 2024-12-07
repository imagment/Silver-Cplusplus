#include "silver.hpp"
#include <atomic>
#include <mutex>
#include <thread>

using namespace std;

atomic<bool> VMouse = false;
mutex mouseMutex;

namespace Silver {
namespace Mouse {

int mouseKey;
int cursorPositionX = 0;
int cursorPositionY = 0;
std::string mouseIcon = "🖱️";
bool hideMouse = false;
}; // namespace Mouse
}; // namespace Silver
void VMouseOn(int l, int r, int u, int d, int c) {
  while (VMouse.load()) {
    char det = Silver::Keyboard::getKey();

    {

      lock_guard<mutex> lock(mouseMutex);
      if (det == l) {
        Silver::Mouse::cursorPositionX--;
      } else if (det == r) {
        Silver::Mouse::cursorPositionX++;
      } else if (det == u) {
        Silver::Mouse::cursorPositionY--;
      } else if (det == d) {
        Silver::Mouse::cursorPositionY++;
      } else if (det == c) {
      }
    }
  }
}

void Silver::Mouse::startVMouse(int l, int r, int u, int d, int c) {

  hideMouse = false;
  if (VMouse.load())
    return;

  VMouse.store(true);
  mouseKey = c;
  thread vmouseThread(VMouseOn, l, r, u, d, c);
  vmouseThread.detach();
}

void Silver::Mouse::stopVMouse() {
  hideMouse = true;
  VMouse.store(false);
}

bool Silver::Mouse::wasMouseClicked() {
  if (Silver::Keyboard::keyBuffer == mouseKey) {
    return true;
  }
  return false;
}
