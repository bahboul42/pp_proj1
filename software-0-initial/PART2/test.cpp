#include "threadpool_test.cpp"
#include <string>
#include <iostream>
#include <chrono>
#include <mutex>
#include <functional>
#include <vector>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
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
    void set(unsigned i)
    {
        std::lock_guard<decltype(mutex)> lock(mutex);
        counter = i;
    }

    void signal()
    {
        std::lock_guard<decltype(mutex)> lock(mutex);
        ++counter;
        condition.notify_one();
    }

    void wait()
    {
        std::unique_lock<decltype(mutex)> lock(mutex);
        while (!counter) // Handle spurious wake-ups.
            condition.wait(lock);
        --counter;
    }

    bool try_wait()
    {
        std::lock_guard<decltype(mutex)> lock(mutex);
        if (!counter)
            return false;
        --counter;
        return true;
    }
};
class quit_worker_exception : public std::exception
{
};

semaphore mysemaphore;
static int i = 0;

void producer(int id)
{
    while (true)
    {
        std::stringstream msg1;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);
        mysemaphore.wait();
        msg1 << "Producer thread number " << id << " in" << std::endl;
        std::cout << msg1.str();
        i++;
        std::stringstream msg2;
        msg2 << i << std::endl;
        std::cout << msg2.str();
        std::stringstream msg3;
        msg3 << "Producer out" << std::endl;
        std::cout << msg3.str();
        mysemaphore.signal();
    }
}

void consumer(int id)
{
    while (true)
    {
        std::stringstream msg1;
        std::stringstream msg2;
        std::stringstream msg3;
        
        msg1 << "Consumer thread number " << id << " in" << std::endl;
        msg3 << "Consumer out" << std::endl;
        
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);
        mysemaphore.wait();

        std::cout << msg1.str();
        i--;
        msg2 << i << std::endl;
        std::cout << msg2.str();

        std::cout << msg3.str();

        mysemaphore.signal();
    }
}

int main()
{
    std::cout << "stating operation" << std::endl;
    std::vector<std::thread> pool;
    mysemaphore.set(1);

    for (unsigned i = 0; i < NTHREADS - 2; i++)
        pool.push_back(std::thread(&producer, i));
    pool.push_back(std::thread(&consumer, 3));
    pool.push_back(std::thread(&consumer, 2));

    for (size_t i = 0; i < NTHREADS; i++)
    {
        pool.at(i).join();
    }
}
