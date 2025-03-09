#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer();

    void Start();
    void Pause();
    void Resume();
    void Stop();
    void Reset();
    long long GetElapsedTime() const;

private:
    bool isRunning;
    bool isPaused;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    std::chrono::high_resolution_clock::time_point pauseTime;
    std::chrono::high_resolution_clock::time_point resumeTime;
    long long accumulatedPausedTime;
    long long totalDuration;
};

#endif
