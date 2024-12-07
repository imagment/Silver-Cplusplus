# Function name : stopVMouse

## Function prototype

```cpp
void stopVMouse();
```

## Implementation

```cpp
void Silver::Mouse::stopVMouse() {
  hideMouse = true;
  VMouse.store(false);
}
```

## Description
This function hides the virtual mouse and stops the VMouse thread.

## Example Usage
```cpp
Silver::Mouse::startVMouse('A', 'D', 'W', 'S', ' ');
startGame();
Silver::Mouse::stopVMouse();
```
