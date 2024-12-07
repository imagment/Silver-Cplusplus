#include "silver.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std;

namespace Silver {
namespace Timer {

std::map<std::string, bool> running;
std::map<std::string, long long> duration;
std::map<std::string, std::chrono::high_resolution_clock::time_point> startTime;
std::map<std::string, std::chrono::high_resolution_clock::time_point> lastTime;
std::map<std::string, float> deltaTime;
}; // namespace Timer

}; // namespace Silver

void Silver::Timer::resetTimer(const string name) {
  running[name] = false;
  duration[name] = 0;
}

long long Silver::Timer::getDuration(const string name) {

  if (running.find(name) != running.end() && running.at(name) == true) {
    auto currentTime = chrono::high_resolution_clock::now();
    return duration.at(name) + chrono::duration_cast<chrono::milliseconds>(
                                   currentTime - startTime.at(name))
                                   .count();
  }

  if (duration.find(name) != duration.end()) {
    return duration.at(name);
  }

  return 0;
}

void Silver::Timer::startTimer(const string name) {

  if (running.find(name) == running.end() || running.at(name) == false) {
    startTime[name] = chrono::high_resolution_clock::now();
    running[name] = true;
  }
}

void Silver::Timer::stopTimer(const string name) {
  if (running[name]) {
    auto endTime = chrono::high_resolution_clock::now();
    duration[name] += chrono::duration_cast<chrono::milliseconds>(
                          endTime - startTime.at(name))
                          .count();
    running[name] = false;
  }
}

float Silver::Timer::getDeltaTime(const string name) {
  return deltaTime.at(name);
}

void Silver::Timer::update(const string name) {
  auto currentTime = chrono::high_resolution_clock::now();

  deltaTime[name] =
      chrono::duration<float>(currentTime - lastTime.at(name)).count();

  lastTime[name] = currentTime;
}
