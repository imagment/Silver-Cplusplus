#include "silver.hpp"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

using namespace std;

void Silver::Threading::tRun(function<void()> func) {
  thread t(func);
  t.detach();
}

void Silver::Threading::dRun(function<void()> func, float time) {
  thread t([func, time]() {
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>(time)));
    func();
  });
  t.detach();
}

void Silver::Threading::createThread(const string name, function<void()> func) {
  std::lock_guard<std::mutex> lock(thread_mutex);

  if (threads.find(name) != threads.end())
    return;

  auto threadInfo = std::make_shared<ThreadInfo>();

  threadInfo->t = std::thread([name, func, threadInfo]() {
    std::unique_lock<std::mutex> lock(threadInfo->mtx);
    while (!threadInfo->stop) {
      if (threadInfo->paused) {
        threadInfo->cv.wait(lock);
      } else {
        func();
        break;
      }
    }
  });

  threads[name] = threadInfo;
}

void Silver::Threading::destroyThread(const string name) {
  lock_guard<mutex> lock(thread_mutex);
  if (threads.find(name) != threads.end()) {
    {
      lock_guard<mutex> th_lock(threads[name]->mtx);
      threads[name]->stop = true;
      threads[name]->cv.notify_all();
    }
    if (threads[name]->t.joinable())
      threads[name]->t.join();
    threads.erase(name);
  }
}

void Silver::Threading::stopAllThreads() {
  lock_guard<mutex> lock(thread_mutex);
  for (auto &pair : threads) {
    {
      lock_guard<mutex> th_lock(pair.second->mtx);
      pair.second->stop = true;
      pair.second->cv.notify_all();
    }
    if (pair.second->t.joinable())
      pair.second->t.join();
  }
  threads.clear();
}

void Silver::Threading::pauseThread(const string name) {
  lock_guard<mutex> lock(thread_mutex);
  if (threads.find(name) != threads.end() && !threads[name]->paused) {
    lock_guard<mutex> th_lock(threads[name]->mtx);
    threads[name]->paused = true;
  }
}

void Silver::Threading::resumeThread(const string name) {
  lock_guard<mutex> lock(thread_mutex);
  if (threads.find(name) != threads.end() && threads[name]->paused) {

    lock_guard<mutex> th_lock(threads[name]->mtx);
    threads[name]->paused = false;
    threads[name]->cv.notify_all();
  }
}
