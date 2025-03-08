#ifndef SILVER_KEYBOARD_HPP
#define SILVER_KEYBOARD_HPP

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
