# Function name : isKey

## Function prototype

```cpp
char Silver::Keyboard::isKey(int key);
```

## Implementation

```cpp
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
