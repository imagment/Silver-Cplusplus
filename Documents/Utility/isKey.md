# Function name : isKey

## Function prototype

```cpp
char Silver::Keyboard::getKey();
```

## Implementation

```cpp
bool Silver::Keyboard::isKey(int key) {
  if (!Keyboard::caseSensitive && key >= 'a' && key <= 'z') {
    key -= 'a' - 'A';
  }
  if (keyBuffer == key) {
    return true;
  }
  return false;
}
```

## Description
This function checks the value of the key buffer and check if a key is the most recently pressed key 

## Example Usage
```cpp
Silver::Keyboard::getKey();
if(isKey('s')) {
  std::cout << "The S key has been pressed!" << std::endl;
}
if(isKey('t')) {
  std::cout << "The T key has been pressed!" << std::endl;
}
```
