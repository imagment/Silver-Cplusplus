#include "SilverAnimation.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <chrono>

using namespace std;


void Animation::LoadAnimationFromFile(const string filename) {
  ifstream file(filename);
  if (!file) {
    cerr << "Error opening file: " << filename << endl;
    return;
  }

  vector<string> animationFrames;
  float fps = -1;
  int transition = -1;

  string line;
  while (getline(file, line)) {
    if (line.compare(0, 6, "$write") == 0) {
      string frames = line.substr(7);
      stringstream ss(frames);
      string frame;
      while (getline(ss, frame, ',')) {
        animationFrames.push_back(frame);
      }
    } else if (line.compare(0, 4, "$for") == 0) {
      string command = line.substr(5);
      stringstream ss(command);
      int repeatCount;
      string frame;
      if (ss >> repeatCount >> frame) {
        for (int i = 0; i < repeatCount; ++i) {
          animationFrames.push_back(frame);
        }
      }
    } else if (line.compare(0, 4, "FPS ") == 0) {
      string fpsStr = line.substr(4);
      fps = stof(fpsStr);
    } else if (line.compare(0, 12, "TRANSITION ") == 0) {
      string transitionStr = line.substr(12);
      if (transitionStr == "IMMIDIATE") {
        transition = -1;
      } else {
        transition = stoi(transitionStr);
      }
    }
  }

  this->animation = animationFrames;
  this->fps = fps;
  this->transition = transition;
}
