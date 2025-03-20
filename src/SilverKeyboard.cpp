#include "SilverKeyboard.hpp"
#include "Silver.hpp"
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <unistd.h>
#include <unordered_map>


// Static variables
static std::unordered_map<int, bool> keyStates; // Stores key states
static std::unordered_map<int, bool> keyDownStates; // Tracks if a key was pressed down
static std::unordered_map<int, bool> keyUpStates; // Tracks if a key was released
static bool isInitialized = false;
static int fd; // File descriptor for input device

// Initializes the keyboard input system
void InitializeKeyboardModule() {
  if (!isInitialized) {
    SetNonBlockingMode();

    isInitialized = true;
  }
}

// Polls events and updates states
void PollEvents() {
  struct input_event event;
  while (read(fd, &event, sizeof(event)) > 0) {
    if (event.type == EV_KEY) {
      int key = event.code;
      if (event.value == 1) { // Key press
        keyStates[key] = true;
        keyDownStates[key] = true;
        keyUpStates[key] = false;
      } else if (event.value == 2) { // Key hold
        keyStates[key] = true;
      } else if (event.value == 0) { // Key release
        keyStates[key] = false;
        keyDownStates[key] = false;
        keyUpStates[key] = true;
      }
    }
  }
}

// Checks if a key is pressed
bool IsKey(int key) { return keyStates[key]; }

// Checks if a key is pressed down (once)
bool IsKeyDown(int key) {
  if (keyDownStates[key]) {
    keyDownStates[key] = false; // Reset after processing
    return true;
  }
  return false;
}

// Checks if a key is released
bool IsKeyUp(int key) {
  if (keyUpStates[key]) {
    keyUpStates[key] = false; // Reset after processing
    return true;
  }
  return false;
}
