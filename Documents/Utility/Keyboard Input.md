# Scene Management
## Structure
```cpp
// The Keyboard namespace
namespace Silver {
    namespace Keyboard {
        const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67, escapeKey = 27; // ASCII codes keys that are frequently used
	
        char keyBuffer; // The most recently pressed key
        bool caseSensitive = true;
    }; 
}; 
```
## Functions
```cpp
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
```

## Function explanation
`getKey`: Retrieves a key input, stores it in the `keyBuffer`, and returns the updated value of the `keyBuffer`. <br>
Also no
`isKey`: Checks if a certain key is the most recently pressed key. <br>

## Other members
```cpp
char keyBuffer;
```
The keyBuffer variable stores the value of the most recently pressed key.

```cpp
const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67, escapeKey = 27;
```
These five constants represent the ASCII codes for commonly used keyboard inputs. They are arrow keys and escape keys.

```cpp
bool caseSensitive = true;
```
Determines whether the system treats lowercase and uppercase letters as the same (false) or different (true). For instance, when `caseSensitive` is true, 's' and 'S' keys are treated as different keys.


## Warning
Avoid using the `getKey` function multiple times in a loop.
```
while (1) {
  if(Silver::Keyboard::getKey() == 'S') {
    std::cout << "The S key has been pressed!" << std::endl;
  }
  if(Silver::Keyboard::getKey() == 'T') {
    std::cout << "The T key has been pressed!" << std::endl;
  }
}
```
Instead, this is more efficient.
```
while (1) {
  Silver::Keyboard::getKey();
  if(Silver::Keyboard::isKey('s')) {
    std::cout << "The S key has been pressed!" << std::endl;
  }
  if(Silver::Keyboard::isKey('t')) {
    std::cout << "The T key has been pressed!" << std::endl;
  }
}
```
