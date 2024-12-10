#include "silver.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

// The Keyboard namespace
namespace Silver {
namespace Keyboard {
const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67,
          escapeKey = 27; // ASCII codes keys that are frequently used

char keyBuffer; // The most recently pressed key
char upKeyBuffer;
bool caseSensitive = true;
bool recieved = true;
bool upRecieved = true;
int holdThreshold = 512;
steady_clock::time_point lastKeyPressTime; // To track the last key press time
}; // namespace Keyboard
}; // namespace Silver

char Silver::Keyboard::getKey() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  // Save and modify terminal settings for non-blocking input
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Set non-blocking mode
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  // Get input character
  ch = getchar();

  // Restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  // If a key is pressed, reset the last key press time
  if (ch != EOF) {
    lastKeyPressTime = steady_clock::now(); // Update the time of last key press

    if (!Keyboard::caseSensitive && ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';
    
    if(keyBuffer != ch) {
      upKeyBuffer = keyBuffer;
      keyBuffer = ch;
      recieved = false;
      upRecieved = false;
    }
    return ch;
  }

  if (duration_cast<milliseconds>(steady_clock::now() - lastKeyPressTime).count() >= holdThreshold) {
    recieved = false;
    upRecieved = false;
    upKeyBuffer = keyBuffer;
    keyBuffer = '\0';
  }

  return '\0';
}

bool Silver::Keyboard::isKey(int key) {
  // Get the most recently pressed key and process it
  if (!Keyboard::caseSensitive && key >= 'a' && key <= 'z') {
    key -= 'a' - 'A';
  }

  // If 'key' is the recently pressed key, return true
  if (keyBuffer == key) {
    return true;
  }

  // If not, return false
  return false;
}

bool Silver::Keyboard::isKeyDown(int key) {
  if (isKey(key) && !recieved) {
    recieved = true;
    return true;
  }
  return false;
}

bool Silver::Keyboard::isKeyUp(int key) {
  if (upKeyBuffer == key && !upRecieved) {
    upRecieved = true;
    return true;
  }
  return false;
}
