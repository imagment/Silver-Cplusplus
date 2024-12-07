# Function name : startVMouse

## Function prototype

```cpp
void startVMouse(int l, int r, int u, int d, int c);
```

## Implementation

```cpp
void Silver::Mouse::startVMouse(int l, int r, int u, int d, int c) {
  Silver::Camera::hideMouse = false;
  if (VMouse.load())
    return;

  VMouse.store(true);
  mouseKey = c;
  thread vmouseThread(VMouseOn, l, r, u, d, c);
  vmouseThread.detach();
}
```

## Description
This function gets an object ID and returns the position of that object.

## Example Usage
```cpp
Silver::Mouse::startVMouse('A', 'D', 'W', 'S', ' ');
Silver::Camera::printCam();
```

![屏幕截图 2024-12-07 224325](https://github.com/user-attachments/assets/7b33b3d5-d7c0-495d-91c2-fb987fb58bc7) <br>
This is how it would look like when camera was shown to the console and the mouse pointer is present (`#` is the mouse icon)
