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
These five constants represent the ASCII codes for commonly used keyboard inputs: arrow keys and escape keys.

```cpp
bool caseSensitive = true;
```
Determines whether the system treats lowercase and uppercase letters as the same (false) or different (true). For instance, when `caseSensitive` is true, the 's' and 'S' keys are treated as different.


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
