#ifndef SILVER_THREADING_HPP
#define SILVER_THREADING_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

class SThread {
public:
    SThread();
    ~SThread();

    void Enqueue(std::function<void()> task);
    void StartThread();
    void StopThread();
    void PauseThread();
    void ResumeThread();
    void JoinThread();
    void DetachThread();

    std::queue<std::function<void()>> taskQueue;

private:
    void ThreadFunction();

    std::thread workerThread;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> isPaused;
    std::atomic<bool> isRunning;
};

#endif // STHREAD_HPP
