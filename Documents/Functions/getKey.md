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

  tcgetattr(STDIN_FILENO, & oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, & newt);

  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, & oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    if (ch >= 'a' && ch <= 'z') ch += 'A' - 'a';
    keyBuffer = ch;
    return ch;
  }
  keyBuffer = '\0';
  return '\0';
}
```

## Description
This function gets a key, saves the user's key to keyBuffer, and returns the key that the user had pressed. If the user did not press any key, then it returns a null character 
and keyBuffer also becomes a null character.

## Example Usage
```cpp
while(1) {
  if(Silver::Keyboard::getKey() == 'a') {
    printf("A had been pressed! \n");
  }
}
```

