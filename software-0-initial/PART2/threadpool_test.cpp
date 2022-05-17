// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
#include <functional>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <queue>
#include "tinyraytracer.hh"
#include <sstream>
#include <chrono>

class ThreadPool
{
    

public:
    void Start();
    void Stop();
    void sendMsg(std::stringstream toSend);

private:
    void mythread();
    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
};

void ThreadPool::Start()
{
    const size_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    for (size_t i = 0; i < num_threads; i++){
         threads.push_back(std::thread(mythread));
    }
}


void ThreadPool::mythread()
{
    while (!should_terminate)
    {
        std::stringstream msg;
        msg << "Message"
            << " from thread" << "\n ";
        std::cout << msg.str();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

// void ThreadPool::ThreadLoop() {
//     while (true) {
//         std::function<void()> job;
//         {
//             std::unique_lock<std::mutex> lock(queue_mutex);
//             mutex_condition.wait(lock, [this] {
//                 return !jobs.empty() || should_terminate;
//             });
//             if (should_terminate) {
//                 return;
//             }
//             job = jobs.front();
//             jobs.pop();
//         }
//         job();
//     }
// }

// void ThreadPool::QueueJob(const std::function<void()>& job) {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         jobs.push(job);
//     }
//     mutex_condition.notify_one();
// }

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}