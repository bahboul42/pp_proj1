#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <cassert>
#include <condition_variable>
#include <sstream>

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


// #include <iostream>
// #include <vector>
// #include <chrono>
// #include <thread>
// #include <mutex>
// #include <cassert>
// #include <condition_variable>
// #include <sstream>
// #include <queue>
// #define NTHREADS 4
// #include "tinyraytracer.hh"

// class semaphore
// {
// public:
//   struct angles
//   {
//     float angle_v;
//     float angle_h;
//     float angle_logo;
//   };

//   void set(unsigned i)
//   {
//     std::lock_guard<decltype(mutex)> lock(mutex);
//     counter = i;
//   }

//   void signal()
//   {
//     std::lock_guard<decltype(mutex)> lock(mutex);
//     ++counter;
//     condition.notify_one();
//   }

//   void wait()
//   {
//     std::unique_lock<decltype(mutex)> lock(mutex);
//     while (!counter) // Handle spurious wake-ups.
//       condition.wait(lock);
//     --counter;
//   }

//   void wait_image()
//   {
//     std::unique_lock<decltype(mutex)> lock(mutex);
//     while (!counter || imageBuffer.size() < 0) // Handle spurious wake-ups.
//       condition.wait(lock);
//     --counter;
//   }

//   void wait_angles()
//   {
//     std::unique_lock<decltype(mutex)> lock(mutex);
//     while (!counter || anglesBuffer.size() < 0) // Handle spurious wake-ups.
//       condition.wait(lock);
//     --counter;
//   }

//   bool try_wait()
//   {
//     std::lock_guard<decltype(mutex)> lock(mutex);
//     if (!counter)
//       return false;
//     --counter;
//     return true;
//   }

//   void pushImg(sf::Image img)
//   {
//     imageBuffer.push(img);
//   }

//   void pushAgl(angles agl)
//   {
//     anglesBuffer.push(agl);
//   }

//   angles getAgl()
//   {
//     angles toReturn;
//     toReturn = anglesBuffer.front();
//     anglesBuffer.pop();
//     return toReturn;
//   }

//   sf::Image getImg()
//   {
//     sf::Image toReturn;
//     toReturn = imageBuffer.front();
//     imageBuffer.pop();
//     return toReturn;
//   }
  
// private :

// std::queue<angles>anglesBuffer;
// std::queue<sf::Image> imageBuffer;
// std::mutex mutex;
// std::condition_variable condition;
// unsigned long counter = 0; // Initialized as locked.
// }
// ;
