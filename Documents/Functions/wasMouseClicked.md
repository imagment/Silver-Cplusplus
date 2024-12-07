# Function name : wasMouseClicked

## Function prototype

```cpp
bool wasMouseClicked();
```

## Implementation

```cpp
bool Silver::Mouse::wasMouseClicked() {
  if (Silver::Keyboard::keyBuffer == mouseKey) {
    return true;
  }
  return false;
}

```

## Description
Checks if the most recently pressed key is the key for clicking the mouse

## Example Usage
```cpp
Silver::Keyboard::getKey();
if(Silver::Mouse::wasMouseClicked() {
  std::cout << "The mouse had been clicked!" << std::endl; 
}
```
