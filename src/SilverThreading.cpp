#include "SilverThreading.hpp"
#include <iostream>

SThread::SThread() : isPaused(false), isRunning(false), workerThread() {}

SThread::~SThread() {
    StopThread();
}

void SThread::Enqueue(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(task);
    cv.notify_all();
}

void SThread::StartThread() {
    isRunning = true;
    workerThread = std::thread(&SThread::ThreadFunction, this);
}

void SThread::StopThread() {
    if (isRunning) {
        isRunning = false;
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();  
        }
    }
}

void SThread::PauseThread() {
    isPaused = true;
}

void SThread::ResumeThread() {
    isPaused = false;
    cv.notify_all();
}

void SThread::ThreadFunction() {
    while (isRunning) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            cv.wait(lock, [this] { return !taskQueue.empty() || !isRunning; });

            if (!isRunning) break;

            if (isPaused) {
                cv.wait(lock, [this] { return !isPaused; });
            }

            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            }
        }

        if (task) {
            task();
        }
    }
}

void SThread::JoinThread() {
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void SThread::DetachThread() {
    if (workerThread.joinable()) {
        workerThread.detach();
    }
}
