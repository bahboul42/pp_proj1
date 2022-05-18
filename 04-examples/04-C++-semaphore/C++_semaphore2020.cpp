#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <cassert>
#include <condition_variable>
#include <sstream>
#define NTHREADS 4

std::counting_semaphore<2> mysemaphore;

void mythread(unsigned id) {
  unsigned i = 30;
  std::stringstream msg1, msg2;
  msg1 << "Thread " << id << " entering critical section\n";
  msg2 << "Thread " << id << " exiting critical section\n";
  while (--i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    mysemaphore.acquire();
    std::cout << msg1.str();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << msg2.str();
    mysemaphore.release();
  }
}

int main(void) {
  std::vector<std::thread> ths;
  for (unsigned i = 0; i < NTHREADS; i++)
    ths.push_back(std::thread(&mythread, i));
  for (auto& th : ths)
    th.join();
  return 0;
}
