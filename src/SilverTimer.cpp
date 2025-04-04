#include "SilverTimer.hpp"
#include <chrono>

using namespace std::chrono;

Timer::Timer() 
    : isRunning(false), isPaused(false), accumulatedPausedTime(0) {}

void Timer::Start() {
    if (!isRunning) {
        startTime = high_resolution_clock::now();
        isRunning = true;
        isPaused = false;
        accumulatedPausedTime = 0;
    }
}

void Timer::Pause() {
    if (isRunning && !isPaused) {
        pauseTime = high_resolution_clock::now();
        isPaused = true;
    }
}

void Timer::Resume() {
    if (isPaused) {
        resumeTime = high_resolution_clock::now();
        accumulatedPausedTime += duration_cast<milliseconds>(resumeTime - pauseTime).count();
        isPaused = false;
    }
}

void Timer::Stop() {
    if (isRunning) {
        if (isPaused) {
            Resume();
        }
        auto stopTime = high_resolution_clock::now();
        totalDuration = duration_cast<milliseconds>(stopTime - startTime).count() - accumulatedPausedTime;
        isRunning = false;
    }
}

void Timer::Reset() {
    isRunning = false;
    isPaused = false;
    totalDuration = 0;
    accumulatedPausedTime = 0;
}

long long Timer::GetElapsedTime() const {
    if (isRunning) {
        if (isPaused) {
            return duration_cast<milliseconds>(pauseTime - startTime).count() - accumulatedPausedTime;
        }
        return duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() - accumulatedPausedTime;
    }
    return totalDuration;
}

