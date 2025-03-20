#ifndef SILVER_KEYBOARD_HPP
#define SILVER_KEYBOARD_HPP

// Arrow Keys
#define KEY_UP 103
#define KEY_DOWN 108
#define KEY_LEFT 105
#define KEY_RIGHT 106

// Function Keys
#define KEY_F1 59
#define KEY_F2 60
#define KEY_F3 61
#define KEY_F4 62
#define KEY_F5 63
#define KEY_F6 64
#define KEY_F7 65
#define KEY_F8 66
#define KEY_F9 67
#define KEY_F10 68
#define KEY_F11 87
#define KEY_F12 88

// Escape Keys
#define KEY_ESC 1

// Backspace and Delete Keys
#define KEY_BACKSPACE 14
#define KEY_DELETE 111

// Other special keys (if needed)
#define KEY_ENTER 28
#define KEY_SPACE 57
#include <unordered_map>

// Initializes the keyboard input system
void InitializeKeyboardModule();

// Handles reading events internally
void PollEvents();

// Key state checkers
bool IsKey(int key);     // Checks if a key is pressed
bool IsKeyDown(int key); // Checks if a key is pressed down (once)
bool IsKeyUp(int key);   // Checks if a key is released

#endif
