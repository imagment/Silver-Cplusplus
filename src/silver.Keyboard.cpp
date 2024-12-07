#include "silver.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

// The Keyboard namespace
namespace Silver {
namespace Keyboard {
const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67,
          escapeKey = 27; // ASCII codes keys that are frequently used

char keyBuffer; // The most recently pressed key
bool caseSensitive = true;
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

  // Process input and return
  if (ch != EOF) {
    if (!Keyboard::caseSensitive && ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';
    keyBuffer = ch;
    return ch;
  }

  keyBuffer = '\0';
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
