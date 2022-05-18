#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <cassert>
#include <condition_variable>
#include <sstream>
#define NTHREADS 4

class semaphore
{
private:
  std::mutex mutex;
  std::condition_variable condition;
  unsigned long counter = 0; // Initialized as locked.

public:
  void set(unsigned i) {
    std::lock_guard<decltype(mutex)> lock(mutex);
    counter = i;
  }

  void signal() {
    std::lock_guard<decltype(mutex)> lock(mutex);
    ++counter;
    condition.notify_one();
  }

  void wait() {
    std::unique_lock<decltype(mutex)> lock(mutex);
    while (!counter) // Handle spurious wake-ups.
      condition.wait(lock);
    --counter;
  }

  bool try_wait() {
    std::lock_guard<decltype(mutex)> lock(mutex);
    if (!counter) return false;
    --counter;
    return true;
  }
};

semaphore mysemaphore;

void mythread(unsigned id) {
  unsigned i = 30;
  std::stringstream msg1, msg2;
  msg1 << "Thread " << id << " entering critical section\n";
  msg2 << "Thread " << id << " exiting critical section\n";
  while (--i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    mysemaphore.wait();
    std::cout << msg1.str();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << msg2.str();
    mysemaphore.signal();
  }
}

int main(void) {
  std::vector<std::thread> ths;
  mysemaphore.set(2);
  for (unsigned i = 0; i < NTHREADS; i++)
    ths.push_back(std::thread(&mythread, i));
  for (auto& th : ths)
    th.join();
  return 0;
}
