# Function name : getKey

## Function prototype

```cpp
char Silver::Keyboard::getKey();
```

## Implementation

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
    if (!Keyboard::caseSensitive && ch >= 'a' && ch <= 'z')
      ch += 'A' - 'a';
    keyBuffer = ch;
    return ch;
  }
  keyBuffer = '\0';
  return '\0';
}
```

## Description
This function gets the key and saves it to the key buffer and returns the updated value of the key buffer.

## Example Usage
```cpp
Silver::Keyboard::getKey();
if(Silver::Keyboard::keyBuffer == 's') {
  std::cout << "The S key has been pressed!" << std::endl;
}
if(Silver::Keyboard::keyBuffer == 't') {
  std::cout << "The T key has been pressed!" << std::endl;
}
```
