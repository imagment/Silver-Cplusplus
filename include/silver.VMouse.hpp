#ifndef SILVER_VMOUSE_HPP
#define SILVER_VMOUSE_HPP

#include <string>

void StopVMouse();
void StartVMouse(int leftKey, int rightKey, int upKey, int downKey, int clickKey);
bool WasMouseClicked();

extern int mouseKey;
extern int cursorPositionX;
extern int cursorPositionY;
extern std::string mouseIcon;
extern bool hideMouse;

#endif
