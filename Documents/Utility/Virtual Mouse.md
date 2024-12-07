# Virtual Mouse
## Structure
```cpp
// The Virtual Mouse namespace
namespace Mouse {
  void stopVMouse();
  void startVMouse(int l, int r, int u, int d, int c);
  bool isMouse();
  int mouseKey;
  int cursorPositionX;
  int cursorPositionY;
  bool hideMouse;
  std::string mouseIcon;
};
```

## Explanation
VMouse, pronounced as "virtual mouse," is a utility provided by Silver C++ that displays a mouse icon on the camera. The icon's movement is controlled using the keyboard, simulating mouse actions.

## Functions
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

void Silver::Mouse::stopVMouse() {
  Silver::Camera::hideMouse = true;
  VMouse.store(false);
}

bool Silver::Mouse::wasMouseClicked() {
  if (Silver::Keyboard::keyBuffer == mouseKey) {
    return true;
  }
  return false;
}
```

## Function explanation
`startMouse` : Starts the virtual mouse. It needs five parameters (`int l, int r, int u, int d, int c`) which is the 5 ascii codes for the key to move/click the mouse pointer
`stopVMouse` : Stops the virtual mouse
`wasMouseClicked` : Checks if the most recently pressed key is the key for clicking the mouse

## Other members
```cpp
int cursorPositionX;
int cursorPositionY;
```
They represent the coordinates of the mouse pointer.

```cpp
std::string mouseIcon;
```
This represents the icon of the mouse pointer.

```cpp
int mouseKey;
```
This variable contains the ASCII code of a key that triggers a click action for the VMouse.

```cpp
bool hideMouse;
```
This variable decides whether the mouse pointer will be visible or not.
