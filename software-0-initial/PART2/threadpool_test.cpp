// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

#pragma once
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cassert>

class Function_pool
{

private:
    std::queue<std::function<void()>> m_function_queue;
    std::mutex m_lock;
    std::condition_variable m_data_condition;
    std::atomic<bool> m_accept_functions;

public:

    Function_pool();
    ~Function_pool();
    void push(std::function<void()> func);
    void done();
    void infinite_loop_func();
};

Function_pool::Function_pool() : m_function_queue(), m_lock(), m_data_condition(), m_accept_functions(true)
{
}

Function_pool::~Function_pool()
{
}

void Function_pool::push(std::function<void()> func)
{
    std::unique_lock<std::mutex> lock(m_lock);
    m_function_queue.push(func);
    // when we send the notification immediately, the consumer will try to get the lock , so unlock asap
    lock.unlock();
    m_data_condition.notify_one();
}

void Function_pool::done()
{
    std::unique_lock<std::mutex> lock(m_lock);
    m_accept_functions = false;
    lock.unlock();
    // when we send the notification immediately, the consumer will try to get the lock , so unlock asap
    m_data_condition.notify_all();
    //notify all waiting threads.
}

void Function_pool::infinite_loop_func()
{
    std::function<void()> func;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(m_lock);
            m_data_condition.wait(lock, [this]() {return !m_function_queue.empty() || !m_accept_functions; });
            if (!m_accept_functions && m_function_queue.empty())
            {
                //lock will be release automatically.
                //finish the thread loop and let it join in the main thread.
                return;
            }
            func = m_function_queue.front();
            m_function_queue.pop();
            //release the lock
        }
        func();
    }
}

// #include <functional>
// #include <mutex>
// #include <iostream>
// #include <condition_variable>
// #include <thread>
// #include <queue>
// #include "tinyraytracer.hh"
// #include <sstream>
// #include <chrono>

// class ThreadPool
// {

// public:
//     void Start();
//     void Stop();
//     void QueueJob(const std::function<void()>& job);

// private:
//     void ThreadLoop();
//     void mythread();
//     bool should_terminate = false;           // Tells threads to stop looking for jobs
//     std::mutex queue_mutex;                  // Prevents data races to the job queue
//     std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
//     std::vector<std::thread> threads;
//     std::queue<std::stringstream> msgs;
//     std::queue<std::function<void()>> jobs;
// };

// void ThreadPool::Start()
// {
//     const size_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
//     for (size_t i = 0; i < num_threads; i++)
//     {
//         threads.push_back(std::thread(ThreadLoop));
//     }
// }

// void ThreadPool::mythread()
// {
//     while (true)
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         mutex_condition.wait(lock, [this]
//                              { return !msgs.empty() || should_terminate; });
//         auto& test = this->msgs;
//         std::stringstream msg;
//         msgs.pop();
//         msg << "Message" << " from thread" << "\n ";
//         std::cout << msg.str();
//         std::this_thread::sleep_for(std::chrono::seconds(2));
//     }
// }

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

// void ThreadPool::Stop()
// {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         should_terminate = true;
//     }
//     mutex_condition.notify_all();
//     for (std::thread &active_thread : threads)
//     {
//         active_thread.join();
//     }
//     threads.clear();
// }