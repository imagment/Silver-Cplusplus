# Scene Management
## Structure
```cpp
namespace Keyboard {
    const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67, escapeKey = 27;
    char keyBuffer;
}; 
```
## Functions
```cpp
char Silver::Keyboard::getKey() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    if (ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';
    keyBuffer = ch;
    return ch;
  }
  keyBuffer = '\0';
  return '\0';
}

bool Silver::Keyboard::isLastKey(char key) {
  if (keyBuffer == key) {
    return true;
  }
  return false;
}
```

## Function explanation
`getKey`: Retrieves a key input, stores it in the `keyBuffer`, and returns the updated value of the `keyBuffer`.
`isKey`: Get the workspace in the `scenes` map and set that map to the current workspace. <br>

## Other members
```cpp
char keyBuffer;
```
The keyBuffer variable stores the value of the most recently pressed key.

```cpp
const int upKey = 65, downKey = 66, leftKey = 68, rightKey = 67, escapeKey = 27;
```
These 5 constants represent the ASCII codes for commonly used keyboard inputs. They are arrow keys and escape keys.

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
  if(Silver::Keyboard::keyBuffer == 's') {
    std::cout << "The S key has been pressed!" << std::endl;
  }
  if(Silver::Keyboard::keyBuffer == 't') {
    std::cout << "The T key has been pressed!" << std::endl;
  }
}
```
